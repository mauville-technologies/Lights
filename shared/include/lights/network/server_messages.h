//
// Created by ozzadar on 2024-12-17.
//

#pragma once

#include <string>
#include <asio.hpp>
#include <vector>
#include <spdlog/spdlog.h>
#include <lights/util/bytes.h>

enum class ServerMessageType : uint8_t {
    Unknown,
    AuthenticationFailed,
    ClientConnected,
    AccountLoggedInElsewhere,
};

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

struct ClientConnectedMessage {
    explicit ClientConnectedMessage(std::string InMessage) {
        Message = std::move(InMessage);
        Length = Message.size();
    }

    const std::string& GetMessage() const {
        return Message;
    }

    static ServerMessageType GetMessageType() {
        return ServerMessageType::ClientConnected;
    }

    operator std::vector<uint8_t>() {
        std::vector<uint8_t> bytes;
        bytes.push_back(static_cast<uint8_t>(GetMessageType()));

        for (auto i = 0; i < sizeof(size_t); ++i) {
            auto byte = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&Length))[i];
            bytes.push_back(byte);
        }

        for (const char& c : Message) {
            bytes.push_back(c);
        }
        return bytes;
    }

    static ClientConnectedMessage Deserialize(asio::ip::tcp::socket& socket) {
        ClientConnectedMessage message;

        asio::error_code ec;
        // the connection request has a length to determine the rest of the message
        socket.read_some(asio::buffer(&message.Length, sizeof(size_t)), ec);

        if (ec) {
            spdlog::error("Error reading server message length: {}", ec.message());
        }

        message.Message.resize(message.Length);
        socket.read_some(asio::buffer(message.Message.data(), message.Length), ec);

        return message;
    }

    operator std::string() const {
        std::string str = "ClientConnectedMessage: ";
        return str + Message;
    }

private:
    // Default constructor for deserialization
    ClientConnectedMessage() = default;

    size_t Length {0};
    std::string Message;
};