#pragma once

#include "connection_delegate.h"
#include "lights/core/net/web_socket.h"

#include <memory>
#include <string>

namespace OZZ::net::client {

    // Owns the client's single connection to a server and dispatches its events through
    // a swappable ConnectionDelegate. Mirrors server::ConnectionHandler's delegate model;
    // unlike the server side, there's exactly one connection and no reactor thread to pin
    // to -- everything here runs on whatever thread calls Poll().
    class ConnectionHandler {
    public:
        ConnectionHandler();

        // Installs the message handler. Safe to call before Connect() (initial phase) or
        // from inside a delegate's own OnMessage() (phase transition, e.g. auth -> gameplay).
        void SetDelegate(std::shared_ptr<ConnectionDelegate> newDelegate);

        // Makes exactly one connection attempt -- see WebSocket::start(). Replaces the
        // underlying socket, so this also how callers retry after a close/error.
        void Connect(const std::string& url);
        void Disconnect();

        void Send(std::string payload);

        // Pumps the underlying WebSocket and dispatches any queued events to the
        // delegate. Call once per frame from the game/update thread.
        void Poll();

        bool IsConnected() const { return connected; }

    private:
        void onWebSocketMessage(const WebSocketMessage& msg);

        std::unique_ptr<WebSocket> ws;
        std::shared_ptr<ConnectionDelegate> delegate;
        bool connected{false};
    };

} // namespace OZZ::net::client
