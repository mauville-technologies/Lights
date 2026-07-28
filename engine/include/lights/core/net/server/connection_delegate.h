#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace OZZ::net::server {

    // Application-defined message handling for one connection. ConnectionHandler owns
    // exactly one delegate at a time and always dispatches through it -- SetDelegate()
    // swaps in a new one (e.g. auth -> gameplay) without ConnectionHandler needing to
    // know phases exist.
    class ConnectionDelegate {
    public:
        virtual ~ConnectionDelegate() = default;

        // Fires exactly once, right after the handshake completes, on whichever delegate
        // the connection factory installed. NOT re-fired across SetDelegate() transitions --
        // a delegate installed via a transition should do "just became active" setup in
        // its own constructor instead.
        virtual void OnOpen() {}

        virtual void OnMessage(std::span<const uint8_t> data, bool binary) = 0;

        virtual void OnClose(const std::string& reason) {}
    };

} // namespace OZZ::net::server
