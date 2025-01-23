//
// Created by ozzadar on 2024-12-30.
//

#pragma once
#include <vector>
#include <asio.hpp>
#include <spdlog/spdlog.h>

#include "message_types.h"

namespace OZZ::network::messages::server {
    struct AuthenticationSuccessful {
        explicit AuthenticationSuccessful(std::string InUsername) {
            Username = std::move(InUsername);
            UsernameLength = Username.size();
        }

        const std::string& GetUsername() const {
            return Username;
        }

        static ServerMessageType GetMessageType() {
            return ServerMessageType::AuthenticationSuccessful;
        }

        operator std::vector<uint8_t>() {
            std::vector<uint8_t> bytes;
            bytes.push_back(static_cast<uint8_t>(GetMessageType()));

            for (auto i = 0; i < sizeof(size_t); ++i) {
                auto byte = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&UsernameLength))[i];
                bytes.push_back(byte);
            }

            for (const char& c : Username) {
                bytes.push_back(c);
            }
            return bytes;
        }

        static AuthenticationSuccessful Deserialize(asio::ip::tcp::socket& socket) {
            AuthenticationSuccessful message;

            asio::error_code ec;
            // the connection request has a length to determine the rest of the message
            socket.read_some(asio::buffer(&message.UsernameLength, sizeof(size_t)), ec);

            if (ec) {
                spdlog::error("Error reading server message length: {}", ec.message());
            }

            message.Username.resize(message.UsernameLength);
            socket.read_some(asio::buffer(message.Username.data(), message.UsernameLength), ec);

            return message;
        }

        operator std::string() const {
            std::string str = "ClientConnectedMessage: ";
            return str + Username;
        }

    private:
        // Default constructor for deserialization
        AuthenticationSuccessful() = default;

        size_t UsernameLength {0};
        std::string Username;
    };
}
