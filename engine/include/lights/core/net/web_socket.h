//
// Cross-platform WebSocket client abstraction.
//
// Backend is chosen at build time: Boost::Beast/Asio on desktop, the browser's
// WebSocket on Emscripten. Both enqueue events and deliver them only from
// poll(), so callbacks always fire on the caller's polling thread.
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

    // Mirrors the previous IXWebSocket message model so both backends map cleanly onto it.
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

        // Makes exactly one connection attempt — neither backend retries on its
        // own. Reconnect policy (if any) is the caller's responsibility, driven
        // by observing Close/Error events from poll() and calling start() again.
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
