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
#include <functional>
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

    // Drives a handler single-threaded (Poll() interleaved with the condition check) until
    // `condition` holds or `timeout` elapses; avoids racing a background poll thread when
    // reading the handler's lifecycle state directly.
    bool PollUntil(ConnectionHandler& handler, const std::function<bool()>& condition,
                   std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (std::chrono::steady_clock::now() < deadline) {
            handler.Poll();
            if (condition())
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return false;
    }

    // Swaps itself out for `next` from inside its own OnMessage(). It is only owned by the
    // handler, so this drops the last reference to the object still executing.
    class SwapOnMessageDelegate final : public ConnectionDelegate {
    public:
        SwapOnMessageDelegate(ConnectionHandler& handler, std::shared_ptr<ConnectionDelegate> next)
            : handler(handler)
            , next(std::move(next)) {}

        void OnMessage(std::span<const uint8_t> data, bool binary) override {
            handler.SetDelegate(std::move(next));
            // Touch members after the swap: would be a use-after-free without the keep-alive.
            ++messagesSeen;
        }

        int messagesSeen{0};

    private:
        ConnectionHandler& handler;
        std::shared_ptr<ConnectionDelegate> next;
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

TEST(ClientConnectionHandlerTest, LifecycleTracksConnectingThenConnectedWithNoDelegate) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    ConnectionHandler handler;
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Disconnected);
    EXPECT_FALSE(handler.HasConnectedBefore());

    handler.Connect(server.url());
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Connecting);

    ASSERT_TRUE(PollUntil(handler, [&] { return handler.IsConnected(); }));
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Connected);
    EXPECT_TRUE(handler.HasConnectedBefore());
}

TEST(ClientConnectionHandlerTest, DisconnectResetsLifecycleButKeepsHasConnectedBefore) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    ConnectionHandler handler;
    handler.Connect(server.url());
    ASSERT_TRUE(PollUntil(handler, [&] { return handler.IsConnected(); }));

    handler.Disconnect();
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Disconnected);
    EXPECT_TRUE(handler.HasConnectedBefore());
}

TEST(ClientConnectionHandlerTest, LifecycleSurvivesDelegateSwapAndDropIsSeenBySecondDelegate) {
    std::shared_ptr<OZZ::net::server::ConnectionHandler> serverConn;
    OZZ::net::server::testing::TestServer server([&](OZZ::net::server::ConnectionHandler& conn) {
        serverConn = conn.shared_from_this();
        return std::make_shared<EchoServerDelegate>(conn);
    });

    ConnectionHandler handler;
    auto first = std::make_shared<RecordingDelegate>();
    handler.SetDelegate(first);
    handler.Connect(server.url());
    ASSERT_TRUE(PollUntil(handler, [&] { return first->WaitForOpen(std::chrono::milliseconds(0)); }));

    auto second = std::make_shared<RecordingDelegate>();
    handler.SetDelegate(second);
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Connected);

    serverConn->Kill("test done");
    serverConn.reset();

    ASSERT_TRUE(PollUntil(handler, [&] { return second->WaitForClose(std::chrono::milliseconds(0)); }));
    EXPECT_EQ(handler.GetLifecycle(), ConnectionLifecycle::Disconnected);
    EXPECT_TRUE(handler.HasConnectedBefore());
}

TEST(ClientConnectionHandlerTest, DelegateMaySwapItselfFromInsideOnMessage) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    ConnectionHandler handler;
    auto next = std::make_shared<RecordingDelegate>();
    handler.SetDelegate(std::make_shared<SwapOnMessageDelegate>(handler, next));
    handler.Connect(server.url());
    ASSERT_TRUE(PollUntil(handler, [&] { return handler.IsConnected(); }));

    // The first echo is handled by the swapping delegate (which then no longer exists);
    // the second must reach the replacement in the same session.
    handler.Send("first");
    handler.Send("second");

    std::optional<std::pair<std::string, bool>> received;
    ASSERT_TRUE(PollUntil(handler, [&] {
        received = next->WaitForMessage(std::chrono::milliseconds(0));
        return received.has_value();
    }));
    EXPECT_EQ(received->first, "second");
}
