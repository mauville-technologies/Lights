//
// Web WebSocket backend — wraps the browser's WebSocket via
// <emscripten/websocket.h>.
//
// Emscripten delivers events on the main (browser) thread. We still enqueue
// them and deliver from poll() so the semantics match the desktop backend:
// callbacks fire on the thread that calls poll().
//
// Link requirement: -lwebsocket.js (added in the client's link options).
//
#ifdef __EMSCRIPTEN__

#include "lights/core/net/web_socket.h"

#include <deque>

#include <emscripten/websocket.h>

namespace OZZ::net {

    namespace {
        class EmscriptenWebSocket final : public WebSocket {
        public:
            ~EmscriptenWebSocket() override {
                if (socket > 0) {
                    emscripten_websocket_close(socket, 1000, "client shutdown");
                    emscripten_websocket_delete(socket);
                }
            }

            void setUrl(const std::string& u) override { url = u; }
            void setOnMessageCallback(OnMessageCallback callback) override {
                userCallback = std::move(callback);
            }

            void start() override {
                if (!emscripten_websocket_is_supported()) {
                    WebSocketMessage err;
                    err.type = WebSocketMessageType::Error;
                    err.errorInfo.reason = "WebSocket API not supported by this browser";
                    queue.push_back(std::move(err));
                    return;
                }

                EmscriptenWebSocketCreateAttributes attrs;
                emscripten_websocket_init_create_attributes(&attrs);
                attrs.url = url.c_str();

                socket = emscripten_websocket_new(&attrs);
                if (socket <= 0) {
                    WebSocketMessage err;
                    err.type = WebSocketMessageType::Error;
                    err.errorInfo.reason = "failed to create WebSocket";
                    queue.push_back(std::move(err));
                    return;
                }

                emscripten_websocket_set_onopen_callback(socket, this, &EmscriptenWebSocket::onOpen);
                emscripten_websocket_set_onerror_callback(socket, this, &EmscriptenWebSocket::onError);
                emscripten_websocket_set_onclose_callback(socket, this, &EmscriptenWebSocket::onClose);
                emscripten_websocket_set_onmessage_callback(socket, this, &EmscriptenWebSocket::onMessage);
            }

            void stop() override {
                if (socket > 0) {
                    emscripten_websocket_close(socket, 1000, "client stop");
                }
            }

            void sendBinary(const std::string& data) override {
                if (socket > 0) {
                    emscripten_websocket_send_binary(
                        socket, const_cast<char*>(data.data()), static_cast<uint32_t>(data.size()));
                }
            }

            void sendText(const std::string& data) override {
                if (socket > 0) {
                    emscripten_websocket_send_utf8_text(socket, data.c_str());
                }
            }

            void poll() override {
                std::deque<WebSocketMessage> drained;
                drained.swap(queue);
                if (!userCallback) return;
                for (const auto& msg : drained) {
                    userCallback(msg);
                }
            }

        private:
            static EM_BOOL onOpen(int, const EmscriptenWebSocketOpenEvent* e, void* userData) {
                auto* self = static_cast<EmscriptenWebSocket*>(userData);
                WebSocketMessage msg;
                msg.type = WebSocketMessageType::Open;
                msg.openInfo.uri = self->url;
                self->queue.push_back(std::move(msg));
                return EM_TRUE;
            }

            static EM_BOOL onError(int, const EmscriptenWebSocketErrorEvent*, void* userData) {
                auto* self = static_cast<EmscriptenWebSocket*>(userData);
                WebSocketMessage msg;
                msg.type = WebSocketMessageType::Error;
                msg.errorInfo.reason = "websocket error";
                self->queue.push_back(std::move(msg));
                return EM_TRUE;
            }

            static EM_BOOL onClose(int, const EmscriptenWebSocketCloseEvent* e, void* userData) {
                auto* self = static_cast<EmscriptenWebSocket*>(userData);
                WebSocketMessage msg;
                msg.type = WebSocketMessageType::Close;
                msg.closeInfo.code   = e->code;
                msg.closeInfo.reason = e->reason;
                msg.closeInfo.remote = !e->wasClean;
                self->queue.push_back(std::move(msg));
                return EM_TRUE;
            }

            static EM_BOOL onMessage(int, const EmscriptenWebSocketMessageEvent* e, void* userData) {
                auto* self = static_cast<EmscriptenWebSocket*>(userData);
                WebSocketMessage msg;
                msg.type   = WebSocketMessageType::Message;
                msg.binary = !e->isText;
                msg.data.assign(reinterpret_cast<const char*>(e->data), e->numBytes);
                self->queue.push_back(std::move(msg));
                return EM_TRUE;
            }

            std::string                  url;
            EMSCRIPTEN_WEBSOCKET_T        socket{0};
            OnMessageCallback            userCallback;
            std::deque<WebSocketMessage> queue;
        };
    } // namespace

    std::unique_ptr<WebSocket> CreateWebSocket() {
        return std::make_unique<EmscriptenWebSocket>();
    }

} // namespace OZZ::net

#endif // __EMSCRIPTEN__
