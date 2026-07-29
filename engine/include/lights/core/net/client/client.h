#pragma once

#include "connection_delegate.h"
#include "connection_handler.h"

#include <functional>
#include <memory>
#include <string>

namespace OZZ::net::client {

    // Builds the delegate for a connection attempt. Supplied by the app at Client
    // construction time -- keeps Client itself free of app-specific knowledge (mirrors
    // server::ConnectionFactory / server::ConnectionRegistry).
    using ConnectionFactory = std::function<std::shared_ptr<ConnectionDelegate>(ConnectionHandler&)>;

    // Top-level client bootstrap, mirroring server::Server: owns the connection and
    // (re)installs a freshly factory-built delegate on every Start() attempt, the same
    // way ConnectionRegistry::NewConnection builds a fresh delegate per accepted socket.
    // There's no accept loop or reactor pool -- a client dials exactly one connection and
    // drives it entirely from Poll().
    class Client {
    public:
        Client(std::string url, ConnectionFactory factory);

        // Makes exactly one connection attempt -- see ConnectionHandler::Connect(). Safe to
        // call again (e.g. for a reconnect); each call gets a fresh delegate from the
        // factory rather than reusing whatever phase the previous connection ended in.
        void Start();
        void Stop();

        bool IsConnected() const;

        // Pumps the underlying connection and dispatches any queued events to the
        // delegate. Call once per frame from the game/update thread.
        void Poll();

    private:
        std::string url;
        ConnectionFactory factory;
        ConnectionHandler conn;
    };

} // namespace OZZ::net::client
