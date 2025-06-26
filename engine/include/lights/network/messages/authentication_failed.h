//
// Created by ozzadar on 2024-12-30.
//

#pragma once
#include <vector>
#include <asio.hpp>

#include "message_types.h"

namespace OZZ::network::messages::server {
    struct AuthenticationFailedMessage {
        static ServerMessageType GetMessageType() {
            return ServerMessageType::AuthenticationFailed;
        }

        operator std::vector<uint8_t>() {
            std::vector<uint8_t> bytes;
            bytes.push_back(static_cast<uint8_t>(GetMessageType()));
            return bytes;
        }

        static AuthenticationFailedMessage Deserialize(asio::ip::tcp::socket& socket) {
            return AuthenticationFailedMessage();
        }
    };
}
