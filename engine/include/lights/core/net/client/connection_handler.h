#pragma once

#include "connection_delegate.h"
#include "lights/core/net/web_socket.h"

#include <cstdint>
#include <memory>
#include <string>

namespace OZZ::net::client {

    enum class ConnectionLifecycle : uint8_t {
        Disconnected,
        Connecting,
        Connected,
    };

    // Owns the client's single connection to a server and dispatches its events through
    // a swappable ConnectionDelegate. Mirrors server::ConnectionHandler's delegate model;
    // unlike the server side, there's exactly one connection and no reactor thread to pin
    // to -- everything here runs on whatever thread calls Poll().
    //
    // Also tracks the connection lifecycle from the Open/Close/Error events it sees, so the
    // state survives SetDelegate() swaps and updates even with no delegate installed.
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

        ConnectionLifecycle GetLifecycle() const { return lifecycle; }

        // True once this handler has reached Connected at least once.
        bool HasConnectedBefore() const { return hasConnectedBefore; }

        bool IsConnected() const { return lifecycle == ConnectionLifecycle::Connected; }

    private:
        void onWebSocketMessage(const WebSocketMessage& msg);

        std::unique_ptr<WebSocket> ws;
        std::shared_ptr<ConnectionDelegate> delegate;
        ConnectionLifecycle lifecycle{ConnectionLifecycle::Disconnected};
        bool hasConnectedBefore{false};
    };

} // namespace OZZ::net::client
