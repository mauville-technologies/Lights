//
// Created by ozzadar on 2024-12-30.
//

#pragma once
#include <vector>
#include <asio.hpp>

#include "message_types.h"

namespace OZZ::network::messages::server {
    struct AccountLoggedInElsewhereMessage {
        static ServerMessageType GetMessageType() {
            return ServerMessageType::AccountLoggedInElsewhere;
        }

        operator std::vector<uint8_t>() {
            std::vector<uint8_t> bytes;
            bytes.push_back(static_cast<uint8_t>(GetMessageType()));
            return bytes;
        }

        static AccountLoggedInElsewhereMessage Deserialize(asio::ip::tcp::socket& socket) {
            return AccountLoggedInElsewhereMessage();
        }
    };
}
