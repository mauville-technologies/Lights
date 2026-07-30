#include "lights/core/net/client/connection_delegate.h"
#include "lights/core/net/client/connection_handler.h"
#include "lights/core/net/server/connection_delegate.h"
#include "lights/core/net/server/connection_handler.h"

#include "harness/test_server.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>

using namespace OZZ::net::client;

namespace {

    // Records everything that happens to it and lets test bodies block-and-wait, since
    // client::ConnectionHandler only delivers events from Poll().
    class RecordingDelegate : public ConnectionDelegate {
    public:
        void OnOpen() override {
            std::lock_guard lock(mutex);
            opened = true;
            cv.notify_all();
        }

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            std::lock_guard lock(mutex);
            messages.emplace_back(std::string(data.begin(), data.end()), binary);
            cv.notify_all();
        }

        void OnClose(const std::string& reason) override {
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

    private:
        std::mutex mutex;
        std::condition_variable cv;
        bool opened{false};
        bool closed{false};
        std::deque<std::pair<std::string, bool>> messages;
    };

    // Polls a ConnectionHandler continuously on a background thread, mirroring how
    // OZZ::net::testing::TestClient drives the raw WebSocket for tests.
    class PollingConnectionHandler {
    public:
        PollingConnectionHandler() {
            running = true;
            pollThread = std::thread([this] {
                while (running) {
                    handler.Poll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        }

        ~PollingConnectionHandler() {
            running = false;
            if (pollThread.joinable()) pollThread.join();
        }

        PollingConnectionHandler(const PollingConnectionHandler&) = delete;
        PollingConnectionHandler& operator=(const PollingConnectionHandler&) = delete;

        ConnectionHandler handler;

    private:
        std::atomic<bool> running{false};
        std::thread pollThread;
    };

    // Server-side delegate that echoes every message back to the client under test.
    class EchoServerDelegate final : public OZZ::net::server::ConnectionDelegate {
    public:
        explicit EchoServerDelegate(OZZ::net::server::ConnectionHandler& conn) : conn(conn) {}

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            conn.Send(std::string(data.begin(), data.end()));
        }

    private:
        OZZ::net::server::ConnectionHandler& conn;
    };

} // namespace

TEST(ClientConnectionHandlerTest, OpenFiresOnConnect) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    auto delegate = std::make_shared<RecordingDelegate>();
    PollingConnectionHandler client;
    client.handler.SetDelegate(delegate);
    client.handler.Connect(server.url());

    EXPECT_TRUE(delegate->WaitForOpen());
}

TEST(ClientConnectionHandlerTest, BinaryMessageRoundTrips) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    auto delegate = std::make_shared<RecordingDelegate>();
    PollingConnectionHandler client;
    client.handler.SetDelegate(delegate);
    client.handler.Connect(server.url());

    ASSERT_TRUE(delegate->WaitForOpen());
    client.handler.Send("hello");

    auto received = delegate->WaitForMessage();
    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(received->first, "hello");
    EXPECT_TRUE(received->second);
}

TEST(ClientConnectionHandlerTest, OnCloseFiresWhenServerCloses) {
    std::shared_ptr<OZZ::net::server::ConnectionHandler> serverConn;
    OZZ::net::server::testing::TestServer server([&](OZZ::net::server::ConnectionHandler& conn) {
        serverConn = conn.shared_from_this();
        return std::make_shared<EchoServerDelegate>(conn);
    });

    auto delegate = std::make_shared<RecordingDelegate>();
    PollingConnectionHandler client;
    client.handler.SetDelegate(delegate);
    client.handler.Connect(server.url());

    ASSERT_TRUE(delegate->WaitForOpen());
    serverConn->Kill("test done");
    // Drop our extra reference now, while the server's io_contexts are still alive:
    // ConnectionHandler's dtor touches Beast's per-context service, so releasing the
    // last owner after TestServer has torn its reactors down would be UB.
    serverConn.reset();

    EXPECT_TRUE(delegate->WaitForClose());
}

TEST(ClientConnectionHandlerTest, SetDelegateSwapsSubsequentMessageHandling) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    auto firstDelegate = std::make_shared<RecordingDelegate>();
    PollingConnectionHandler client;
    client.handler.SetDelegate(firstDelegate);
    client.handler.Connect(server.url());

    ASSERT_TRUE(firstDelegate->WaitForOpen());
    client.handler.Send("first message");

    auto firstReceived = firstDelegate->WaitForMessage();
    ASSERT_TRUE(firstReceived.has_value());
    EXPECT_EQ(firstReceived->first, "first message");

    auto secondDelegate = std::make_shared<RecordingDelegate>();
    client.handler.SetDelegate(secondDelegate);
    client.handler.Send("second message");

    auto secondReceived = secondDelegate->WaitForMessage();
    ASSERT_TRUE(secondReceived.has_value());
    EXPECT_EQ(secondReceived->first, "second message");
}
