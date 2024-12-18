//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <lights/util/bytes.h>

enum class ClientMessageType : uint8_t {
    Unknown,
    ConnectionRequest
};

struct ConnectionRequestMessage {
    explicit ConnectionRequestMessage(std::string InEmail, std::string InPassword) {
        Email = std::move(InEmail);
        EmailLength = Email.size();
        Password = std::move(InPassword);
        PasswordLength = Password.size();
    }

    const std::string& GetEmail() const {
        return Email;
    }

    const std::string& GetPassword() const {
        return Password;
    }

    static ClientMessageType GetMessageType() {
        return ClientMessageType::ConnectionRequest;
    }

    static std::vector<uint8_t> Serialize(const ConnectionRequestMessage& inMessage) {
        std::vector<uint8_t> bytes;
        bytes.push_back(static_cast<uint8_t>(GetMessageType()));

        for (auto i = 0; i < sizeof(size_t); ++i) {
            auto byte = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&inMessage.EmailLength))[i];
            bytes.push_back(byte);
        }

        for (const char& c : inMessage.Email) {
            bytes.push_back(c);
        }

        for (auto i = 0; i < sizeof(size_t); ++i) {
            auto byte = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&inMessage.PasswordLength))[i];
            bytes.push_back(byte);
        }

        for (const char& c : inMessage.Password) {
            bytes.push_back(c);
        }
        return bytes;
    }

    static ConnectionRequestMessage Deserialize(asio::ip::tcp::socket& socket) {
        ConnectionRequestMessage message;

        asio::error_code ec;
        // the connection request has a length to determine the rest of the message
        socket.read_some(asio::buffer(&message.EmailLength, sizeof(size_t)), ec);

        if (ec) {
            spdlog::error("Error reading email length: {}", ec.message());
        }

        message.Email.resize(message.EmailLength);
        socket.read_some(asio::buffer(message.Email.data(), message.EmailLength), ec);

        socket.read_some(asio::buffer(&message.PasswordLength, sizeof(size_t)), ec);

        if (ec) {
            spdlog::error("Error reading password length: {}", ec.message());
        }

        message.Password.resize(message.PasswordLength);
        socket.read_some(asio::buffer(message.Password.data(), message.PasswordLength), ec);
        return message;
    }

    operator std::string() const {
        std::string str = "ConnectionRequestMessage: ";
        return str + Email + " | " + Password;
    }

private:
    // Default constructor for deserialization
    ConnectionRequestMessage() = default;

    size_t EmailLength {0};
    std::string Email;
    size_t PasswordLength {0};
    std::string Password;
};
