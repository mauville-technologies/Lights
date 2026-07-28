#pragma once

#include "lights/core/net/web_socket.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

namespace OZZ::net::testing {

    // Thin synchronous wrapper around OZZ::net::WebSocket for tests: the client backend
    // only delivers events from poll(), so this spins a background thread that polls
    // continuously and lets test bodies block-and-wait for the next event instead of
    // juggling callbacks.
    class TestClient {
    public:
        explicit TestClient(const std::string& url) {
            ws = CreateWebSocket();
            ws->setUrl(url);
            ws->setOnMessageCallback([this](const WebSocketMessage& msg) {
                std::lock_guard lock(mutex);
                events.push_back(msg);
                cv.notify_all();
            });
            ws->start();

            running = true;
            pollThread = std::thread([this] {
                while (running) {
                    ws->poll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
        }

        ~TestClient() {
            running = false;
            if (pollThread.joinable()) pollThread.join();
            ws->stop();
        }

        TestClient(const TestClient&) = delete;
        TestClient& operator=(const TestClient&) = delete;

        void sendBinary(const std::string& payload) { ws->sendBinary(payload); }
        void sendText(const std::string& payload) { ws->sendText(payload); }

        // Waits for the next event of a given type, discarding any others (e.g. Open)
        // received first. Returns nullopt if the timeout elapses.
        std::optional<WebSocketMessage> waitForEventType(WebSocketMessageType type,
                                                          std::chrono::milliseconds timeout = std::chrono::seconds(5)) {
            const auto deadline = std::chrono::steady_clock::now() + timeout;

            std::unique_lock lock(mutex);
            while (true) {
                auto it = std::find_if(events.begin(), events.end(), [type](const WebSocketMessage& e) {
                    return e.type == type;
                });
                if (it != events.end()) {
                    WebSocketMessage event = std::move(*it);
                    events.erase(it);
                    return event;
                }

                if (cv.wait_until(lock, deadline) == std::cv_status::timeout) {
                    return std::nullopt;
                }
            }
        }

    private:
        std::unique_ptr<WebSocket> ws;
        std::thread pollThread;
        std::atomic<bool> running{false};
        std::mutex mutex;
        std::condition_variable cv;
        std::deque<WebSocketMessage> events;
    };

} // namespace OZZ::net::testing
