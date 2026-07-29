#pragma once

#include "connection_delegate.h"
#include "connection_handler.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace OZZ::net::client {

    // Builds the delegate for a connection attempt. Supplied by the app at Client
    // construction time -- keeps Client itself free of app-specific knowledge (mirrors
    // server::ConnectionFactory / server::ConnectionRegistry).
    using ConnectionFactory = std::function<std::shared_ptr<ConnectionDelegate>(ConnectionHandler&)>;

    enum class ConnectionLifecycle : uint8_t {
        Disconnected,
        Connecting,
        Connected,
    };

    // Top-level client bootstrap, mirroring server::Server: owns the connection and
    // (re)installs a freshly factory-built delegate on every Start() attempt, the same
    // way ConnectionRegistry::NewConnection builds a fresh delegate per accepted socket.
    // There's no accept loop or reactor pool -- a client dials exactly one connection and
    // drives it entirely from Poll().
    //
    // Client also tracks its own connection lifecycle from the Open/Close/Error events it
    // already sees, since that's transport fact rather than app policy. What to *do* about
    // a drop (retry, backoff, give up) stays the caller's decision -- Client only reports.
    class Client {
    public:
        Client(std::string url, ConnectionFactory factory);

        // Makes exactly one connection attempt -- see ConnectionHandler::Connect(). Safe to
        // call again (e.g. for a reconnect); each call gets a fresh delegate from the
        // factory rather than reusing whatever phase the previous connection ended in.
        void Start();
        void Stop();

        ConnectionLifecycle GetLifecycle() const { return lifecycle; }

        // True once this Client has reached Connected at least once. Lets a caller tell a
        // first connection attempt failing apart from an established connection dropping,
        // without tracking that itself.
        bool HasConnectedBefore() const { return hasConnectedBefore; }

        bool IsConnected() const;

        // Pumps the underlying connection and dispatches any queued events to the
        // delegate. Call once per frame from the game/update thread.
        void Poll();

    private:
        // Wraps the factory-built delegate so Client can observe Open/Close/Error and keep
        // `lifecycle`/`hasConnectedBefore` current, without ConnectionHandler or
        // ConnectionDelegate needing any notion that Client exists.
        class LifecycleDelegate;

        std::string url;
        ConnectionFactory factory;
        ConnectionHandler conn;
        ConnectionLifecycle lifecycle{ConnectionLifecycle::Disconnected};
        bool hasConnectedBefore{false};
    };

} // namespace OZZ::net::client
