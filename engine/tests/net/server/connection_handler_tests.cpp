#include "lights/core/net/server/connection_delegate.h"
#include "lights/core/net/server/connection_handler.h"
#include "lights/core/net/web_socket.h"

#include "harness/test_client.h"
#include "harness/test_server.h"

#include <gtest/gtest.h>

#include <chrono>
#include <condition_variable>
#include <deque>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <utility>

using namespace OZZ::net::server;

namespace {

    // Records everything that happens to it; thread-safe since callbacks run on a
    // reactor thread while assertions run on the test thread.
    class RecordingDelegate : public ConnectionDelegate {
    public:
        explicit RecordingDelegate(ConnectionHandler& conn) : conn(conn) {}

        void OnAttached() override {
            std::lock_guard lock(mutex);
            opened = true;
            cv.notify_all();
        }

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            std::lock_guard lock(mutex);
            messages.emplace_back(std::string(data.begin(), data.end()), binary);
            cv.notify_all();
        }

        void OnDetached(const ConnectionDelegateDetachReason reason) override {
            std::lock_guard lock(mutex);
            closed = true;
            cv.notify_all();
        }

        bool WaitForOpen(std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
            std::unique_lock lock(mutex);
            return cv.wait_for(lock, timeout, [this] { return opened; });
        }

        std::optional<std::pair<std::string, bool>> WaitForMessage(std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
            std::unique_lock lock(mutex);
            if (!cv.wait_for(lock, timeout, [this] { return !messages.empty(); })) {
                return std::nullopt;
            }
            auto msg = messages.front();
            messages.pop_front();
            return msg;
        }

        bool WaitForClose(std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
            std::unique_lock lock(mutex);
            return cv.wait_for(lock, timeout, [this] { return closed; });
        }

        ConnectionHandler& conn;

    private:
        std::mutex mutex;
        std::condition_variable cv;
        bool opened{false};
        bool closed{false};
        std::deque<std::pair<std::string, bool>> messages;
    };

    // Echoes every message back to the sender.
    class EchoDelegate final : public RecordingDelegate {
    public:
        using RecordingDelegate::RecordingDelegate;

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            RecordingDelegate::OnMessage(data, binary);
            conn.Send(std::string(data.begin(), data.end()));
        }
    };

} // namespace

TEST(ConnectionHandlerTest, OpenFiresOnConnect) {
    std::shared_ptr<RecordingDelegate> serverDelegate;
    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        serverDelegate = std::make_shared<RecordingDelegate>(conn);
        return serverDelegate;
    });

    OZZ::net::testing::TestClient client(server.url());
    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    ASSERT_NE(serverDelegate, nullptr);
    EXPECT_TRUE(serverDelegate->WaitForOpen());
}

TEST(ConnectionHandlerTest, BinaryMessageRoundTrips) {
    std::shared_ptr<EchoDelegate> serverDelegate;
    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        serverDelegate = std::make_shared<EchoDelegate>(conn);
        return serverDelegate;
    });

    OZZ::net::testing::TestClient client(server.url());
    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    client.sendBinary("hello");

    auto received = serverDelegate->WaitForMessage();
    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(received->first, "hello");
    EXPECT_TRUE(received->second);

    auto echoed = client.waitForEventType(OZZ::net::WebSocketMessageType::Message);
    ASSERT_TRUE(echoed.has_value());
    EXPECT_EQ(echoed->data, "hello");
    EXPECT_TRUE(echoed->binary);
}

TEST(ConnectionHandlerTest, TextMessageDeliveredAsNonBinary) {
    // Unlike the old app-level ClientConnection (which silently dropped text frames),
    // the generic handler delivers everything -- ignoring non-binary is an app policy now.
    std::shared_ptr<RecordingDelegate> serverDelegate;
    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        serverDelegate = std::make_shared<RecordingDelegate>(conn);
        return serverDelegate;
    });

    OZZ::net::testing::TestClient client(server.url());
    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    client.sendText("hello");

    auto received = serverDelegate->WaitForMessage();
    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(received->first, "hello");
    EXPECT_FALSE(received->second);
}

TEST(ConnectionHandlerTest, KillClosesGracefullyAfterOutboxDrains) {
    class KillOnMessageDelegate final : public RecordingDelegate {
    public:
        using RecordingDelegate::RecordingDelegate;
        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            RecordingDelegate::OnMessage(data, binary);
            conn.Send("last words");
            conn.Kill("test done");
        }
    };

    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        return std::make_shared<KillOnMessageDelegate>(conn);
    });

    OZZ::net::testing::TestClient client(server.url());
    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    client.sendBinary("go");

    auto msg = client.waitForEventType(OZZ::net::WebSocketMessageType::Message);
    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(msg->data, "last words");

    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Close).has_value());
}

TEST(ConnectionHandlerTest, SetDelegateSwapsSubsequentMessageHandling) {
    class SecondDelegate final : public RecordingDelegate {
    public:
        using RecordingDelegate::RecordingDelegate;
        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            RecordingDelegate::OnMessage(data, binary);
            conn.Send("from-second");
        }
    };

    class FirstDelegate final : public RecordingDelegate {
    public:
        FirstDelegate(ConnectionHandler& conn, std::promise<std::shared_ptr<SecondDelegate>> promise)
            : RecordingDelegate(conn), promise(std::move(promise)) {}

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            RecordingDelegate::OnMessage(data, binary);
            auto next = std::make_shared<SecondDelegate>(conn);
            conn.AttachDelegate(next);
            promise.set_value(std::move(next));
        }

    private:
        std::promise<std::shared_ptr<SecondDelegate>> promise;
    };

    std::promise<std::shared_ptr<SecondDelegate>> secondPromise;
    std::future<std::shared_ptr<SecondDelegate>> secondFuture = secondPromise.get_future();

    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        return std::make_shared<FirstDelegate>(conn, std::move(secondPromise));
    });

    OZZ::net::testing::TestClient client(server.url());
    ASSERT_TRUE(client.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    client.sendBinary("switch");

    ASSERT_EQ(secondFuture.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto secondDelegate = secondFuture.get();
    ASSERT_NE(secondDelegate, nullptr);

    client.sendBinary("second message");
    auto reply = client.waitForEventType(OZZ::net::WebSocketMessageType::Message);
    ASSERT_TRUE(reply.has_value());
    EXPECT_EQ(reply->data, "from-second");

    auto secondReceived = secondDelegate->WaitForMessage();
    ASSERT_TRUE(secondReceived.has_value());
    EXPECT_EQ(secondReceived->first, "second message");
}

TEST(ConnectionHandlerTest, MultipleConnectionsAreIndependent) {
    OZZ::net::server::testing::TestServer server([&](ConnectionHandler& conn) {
        return std::make_shared<EchoDelegate>(conn);
    });

    OZZ::net::testing::TestClient clientA(server.url());
    OZZ::net::testing::TestClient clientB(server.url());

    ASSERT_TRUE(clientA.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());
    ASSERT_TRUE(clientB.waitForEventType(OZZ::net::WebSocketMessageType::Open).has_value());

    clientA.sendBinary("from-a");
    clientB.sendBinary("from-b");

    auto msgA = clientA.waitForEventType(OZZ::net::WebSocketMessageType::Message);
    auto msgB = clientB.waitForEventType(OZZ::net::WebSocketMessageType::Message);
    ASSERT_TRUE(msgA.has_value());
    ASSERT_TRUE(msgB.has_value());
    EXPECT_EQ(msgA->data, "from-a");
    EXPECT_EQ(msgB->data, "from-b");
}
