#include "lights/core/net/client/connection_handler.h"

#include <utility>

namespace OZZ::net::client {

    ConnectionHandler::ConnectionHandler() : ws(CreateWebSocket()) {}

    void ConnectionHandler::SetDelegate(std::shared_ptr<ConnectionDelegate> newDelegate) {
        delegate = std::move(newDelegate);
    }

    void ConnectionHandler::Connect(const std::string& url) {
        // Recreate the socket rather than reusing it across attempts: a WebSocket whose
        // start() already ran (even if it later closed/errored) isn't guaranteed to
        // produce a fresh connection attempt from a second start() call, so reconnects
        // need a clean instance each time.
        ws = CreateWebSocket();
        ws->setUrl(url);
        ws->setOnMessageCallback([this](const WebSocketMessage& msg) { onWebSocketMessage(msg); });
        ws->start();
    }

    void ConnectionHandler::Disconnect() {
        connected = false;
        ws->stop();
    }

    void ConnectionHandler::Send(std::string payload) {
        ws->sendBinary(payload);
    }

    void ConnectionHandler::Poll() {
        ws->poll();
    }

    void ConnectionHandler::onWebSocketMessage(const WebSocketMessage& msg) {
        switch (msg.type) {
            case WebSocketMessageType::Open:
                connected = true;
                if (delegate) delegate->OnOpen();
                break;
            case WebSocketMessageType::Close:
                connected = false;
                if (delegate) delegate->OnClose(msg.closeInfo.reason);
                break;
            case WebSocketMessageType::Error:
                connected = false;
                if (delegate) delegate->OnError(msg.errorInfo.reason);
                break;
            case WebSocketMessageType::Message: {
                // Keep the delegate alive for the whole call: OnMessage() may call
                // SetDelegate(), which would otherwise drop the last reference to the
                // object still executing (mirrors server::ConnectionHandler::onRead).
                auto currentDelegate = delegate;
                if (currentDelegate) {
                    currentDelegate->OnMessage(
                        {reinterpret_cast<const uint8_t*>(msg.data.data()), msg.data.size()}, msg.binary);
                }
                break;
            }
        }
    }

} // namespace OZZ::net::client
