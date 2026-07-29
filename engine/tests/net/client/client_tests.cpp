#include "lights/core/net/client/client.h"
#include "lights/core/net/client/connection_delegate.h"
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
    // Client only delivers events from Poll().
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

    private:
        std::mutex mutex;
        std::condition_variable cv;
        bool opened{false};
        std::deque<std::pair<std::string, bool>> messages;
    };

    // Like RecordingDelegate, but also holds the ConnectionHandler the factory handed
    // it, so the test body can send through it -- mirroring how an app delegate (e.g.
    // AuthDelegate) sends via the conn reference it was constructed with.
    class SendingRecordingDelegate final : public RecordingDelegate {
    public:
        explicit SendingRecordingDelegate(ConnectionHandler& conn) : conn(conn) {}

        void Send(std::string payload) { conn.Send(std::move(payload)); }

    private:
        ConnectionHandler& conn;
    };

    // Polls a Client continuously on a background thread, mirroring how
    // OZZ::net::testing::TestClient drives the raw WebSocket for tests.
    class PollingClient {
    public:
        PollingClient(std::string url, ConnectionFactory factory) : client(std::move(url), std::move(factory)) {
            running = true;
            pollThread = std::thread([this] {
                while (running) {
                    client.Poll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        }

        ~PollingClient() {
            running = false;
            if (pollThread.joinable()) pollThread.join();
        }

        PollingClient(const PollingClient&) = delete;
        PollingClient& operator=(const PollingClient&) = delete;

        Client client;

    private:
        std::atomic<bool> running{false};
        std::thread pollThread;
    };

    // Echoes every message back to the client under test.
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

TEST(ClientTest, StartConnectsAndDeliversOpenThroughFactoryDelegate) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    auto delegate = std::make_shared<RecordingDelegate>();
    PollingClient client(server.url(), [delegate](ConnectionHandler&) { return delegate; });
    client.client.Start();

    EXPECT_TRUE(delegate->WaitForOpen());
}

TEST(ClientTest, MessageRoundTripsThroughFactoryDelegate) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    std::shared_ptr<SendingRecordingDelegate> delegate;
    PollingClient client(server.url(), [&](ConnectionHandler& conn) {
        delegate = std::make_shared<SendingRecordingDelegate>(conn);
        return delegate;
    });
    client.client.Start();

    ASSERT_TRUE(delegate->WaitForOpen());
    delegate->Send("hello");

    auto received = delegate->WaitForMessage();
    ASSERT_TRUE(received.has_value());
    EXPECT_EQ(received->first, "hello");
    EXPECT_TRUE(received->second);
}

TEST(ClientTest, StartAgainBuildsFreshDelegateFromFactory) {
    OZZ::net::server::testing::TestServer server([](OZZ::net::server::ConnectionHandler& conn) {
        return std::make_shared<EchoServerDelegate>(conn);
    });

    int factoryCallCount = 0;
    std::shared_ptr<RecordingDelegate> lastDelegate;
    Client client(server.url(), [&](ConnectionHandler&) {
        ++factoryCallCount;
        lastDelegate = std::make_shared<RecordingDelegate>();
        return lastDelegate;
    });

    // Driven single-threaded (no PollingClient) so Start()/Stop() -- which replace the
    // underlying WebSocket -- never race a background thread's concurrent Poll(), the
    // same way the real game loop only ever calls Connect()/Poll() from one thread.
    const auto pollUntilOpen = [&](std::chrono::milliseconds timeout) {
        const auto deadline = std::chrono::steady_clock::now() + timeout;
        while (std::chrono::steady_clock::now() < deadline) {
            client.Poll();
            if (lastDelegate->WaitForOpen(std::chrono::milliseconds(0)))
                return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        return false;
    };

    client.Start();
    ASSERT_TRUE(pollUntilOpen(std::chrono::seconds(5)));
    EXPECT_EQ(factoryCallCount, 1);

    client.Stop();
    client.Start();
    ASSERT_TRUE(pollUntilOpen(std::chrono::seconds(5)));
    EXPECT_EQ(factoryCallCount, 2);
}
