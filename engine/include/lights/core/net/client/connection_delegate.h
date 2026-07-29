#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace OZZ::net::client {

    // Application-defined message handling for the client's connection to a server.
    // ConnectionHandler owns exactly one delegate at a time and always dispatches
    // through it -- SetDelegate() swaps in a new one (e.g. auth -> gameplay) without
    // ConnectionHandler needing to know phases exist. Mirrors server::ConnectionDelegate;
    // OnError is the one addition, since a client dial can fail before ever opening,
    // which has no server-side equivalent.
    class ConnectionDelegate {
    public:
        virtual ~ConnectionDelegate() = default;

        // Fires exactly once per successful connection, right after the socket opens,
        // on whichever delegate ConnectionHandler::Connect() was called with installed.
        virtual void OnOpen() {}

        virtual void OnMessage(std::span<const uint8_t> data, bool binary) = 0;

        virtual void OnClose(const std::string& reason) {}

        virtual void OnError(const std::string& reason) {}
    };

} // namespace OZZ::net::client
