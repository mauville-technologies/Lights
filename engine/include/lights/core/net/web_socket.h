//
// Cross-platform WebSocket client abstraction.
//
// Network code is written ONCE against this interface. The engine selects the
// backend at build time:
//   * Desktop (Windows/macOS/Linux) -> IXWebSocket (native BSD sockets)
//   * Web (Emscripten/WASM)         -> emscripten/websocket.h (the browser's
//                                       native WebSocket object)
//
// The two backends have different threading models (IXWebSocket delivers events
// on a background thread; Emscripten delivers them on the main thread). To keep
// behaviour identical everywhere, both backends ENQUEUE incoming events and only
// deliver them to the user callback from `poll()`, which the caller invokes on
// its own (game) thread once per tick. Callbacks therefore always fire on the
// polling thread on every platform.
//
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace OZZ::net {

    enum class WebSocketMessageType {
        Open,
        Close,
        Error,
        Message,
    };

    struct WebSocketErrorInfo {
        std::string reason;
        int         retries{0};
        uint16_t    httpStatus{0};
    };

    struct WebSocketOpenInfo {
        std::string uri;
        std::string protocol;
    };

    struct WebSocketCloseInfo {
        uint16_t    code{0};
        std::string reason;
        bool        remote{false};
    };

    // Mirrors IXWebSocket's message model so the two backends map cleanly and
    // porting existing IXWebSocket code is mechanical.
    struct WebSocketMessage {
        WebSocketMessageType type{WebSocketMessageType::Message};
        std::string          data;          // payload for Message events
        bool                 binary{false}; // Message: binary vs UTF-8 text
        WebSocketErrorInfo   errorInfo;      // valid for Error
        WebSocketOpenInfo    openInfo;       // valid for Open
        WebSocketCloseInfo   closeInfo;      // valid for Close
    };

    using OnMessageCallback = std::function<void(const WebSocketMessage&)>;

    // Abstract WebSocket client. Create via CreateWebSocket().
    class WebSocket {
    public:
        virtual ~WebSocket() = default;

        virtual void setUrl(const std::string& url) = 0;
        virtual void setOnMessageCallback(OnMessageCallback callback) = 0;

        virtual void start() = 0;
        virtual void stop() = 0;

        virtual void sendBinary(const std::string& data) = 0;
        virtual void sendText(const std::string& data) = 0;

        // Deliver any queued events to the callback on the calling thread.
        // Call once per frame from the game/update thread.
        virtual void poll() = 0;
    };

    // Factory — returns the backend compiled for this platform.
    std::unique_ptr<WebSocket> CreateWebSocket();

} // namespace OZZ::net
