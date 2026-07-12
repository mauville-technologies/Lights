//
// Desktop WebSocket backend — wraps IXWebSocket.
//
// IXWebSocket delivers events on an internal background thread. We translate
// each event into an OZZ::net::WebSocketMessage and push it onto a queue; the
// user callback is invoked later from poll() on the caller's thread, so the
// threading model matches the Emscripten backend.
//
#ifndef __EMSCRIPTEN__

#include "lights/core/net/web_socket.h"

#include <deque>
#include <mutex>

#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketMessageType.h>

namespace OZZ::net {

    namespace {
        class IxWebSocket final : public WebSocket {
        public:
            IxWebSocket() {
                // The Emscripten backend has no concept of automatic reconnection (the
                // browser's WebSocket never retries on its own), so callers of this
                // interface are written against "start() is a single connection
                // attempt; reconnect policy is the caller's job" — see web_socket.h.
                // IXWebSocket defaults to its own background auto-reconnect with
                // exponential backoff, which silently violates that contract on
                // desktop and can leave a caller's own reconnect state machine
                // fighting a socket that's already retrying underneath it.
                ws.disableAutomaticReconnection();
            }

            void setUrl(const std::string& url) override { ws.setUrl(url); }

            void setOnMessageCallback(OnMessageCallback callback) override {
                userCallback = std::move(callback);
            }

            void start() override {
                ws.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
                    WebSocketMessage out;
                    switch (msg->type) {
                        case ix::WebSocketMessageType::Open:
                            out.type = WebSocketMessageType::Open;
                            out.openInfo.uri      = msg->openInfo.uri;
                            out.openInfo.protocol = msg->openInfo.protocol;
                            break;
                        case ix::WebSocketMessageType::Close:
                            out.type = WebSocketMessageType::Close;
                            out.closeInfo.code   = msg->closeInfo.code;
                            out.closeInfo.reason = msg->closeInfo.reason;
                            out.closeInfo.remote = msg->closeInfo.remote;
                            break;
                        case ix::WebSocketMessageType::Error:
                            out.type = WebSocketMessageType::Error;
                            out.errorInfo.reason     = msg->errorInfo.reason;
                            out.errorInfo.retries    = msg->errorInfo.retries;
                            out.errorInfo.httpStatus = msg->errorInfo.http_status;
                            break;
                        case ix::WebSocketMessageType::Message:
                            out.type   = WebSocketMessageType::Message;
                            out.data   = msg->str;
                            out.binary = msg->binary;
                            break;
                        default:
                            return; // Ping/Pong/Fragment: not surfaced
                    }
                    std::lock_guard<std::mutex> lock(queueMutex);
                    queue.push_back(std::move(out));
                });
                ws.start();
            }

            void stop() override { ws.stop(); }

            void sendBinary(const std::string& data) override { ws.sendBinary(data); }
            void sendText(const std::string& data) override { ws.sendText(data); }

            void poll() override {
                std::deque<WebSocketMessage> drained;
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    drained.swap(queue);
                }
                if (!userCallback) return;
                for (const auto& msg : drained) {
                    userCallback(msg);
                }
            }

        private:
            ix::WebSocket                ws;
            OnMessageCallback            userCallback;
            std::mutex                   queueMutex;
            std::deque<WebSocketMessage> queue;
        };
    } // namespace

    std::unique_ptr<WebSocket> CreateWebSocket() {
        return std::make_unique<IxWebSocket>();
    }

} // namespace OZZ::net

#endif // !__EMSCRIPTEN__
