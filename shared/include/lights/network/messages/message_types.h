//
// Created by ozzadar on 2024-12-30.
//

#pragma once
#include <cstdint>

namespace OZZ::network::messages {
    enum class ClientMessageType : uint8_t {
        Unknown,
        AuthenticationRequest
    };

    enum class ServerMessageType : uint8_t {
        Unknown,
        AuthenticationFailed,
        AuthenticationSuccessful,
        AccountLoggedInElsewhere,
    };
}
