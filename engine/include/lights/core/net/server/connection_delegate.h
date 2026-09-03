#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace OZZ::net::server {

    enum ConnectionDelegateDetachReason : uint8_t {
        InitialAttach,
        ChangeDelegate,
        ConnectionClosed,
    };

    // Application-defined message handling for one connection. ConnectionHandler owns
    // exactly one delegate at a time and always dispatches through it -- SetDelegate()
    // swaps in a new one (e.g. auth -> gameplay) without ConnectionHandler needing to
    // know phases exist.
    class ConnectionDelegate {
    public:
        virtual ~ConnectionDelegate() = default;

        virtual void OnAttached() {}

        virtual void OnMessage(std::span<const uint8_t> data, bool binary) = 0;

        virtual void OnDetached(const ConnectionDelegateDetachReason reason) {}
    };

} // namespace OZZ::net::server
