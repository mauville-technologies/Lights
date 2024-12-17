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
    explicit ConnectionRequestMessage(std::string InName) {
        Name = std::move(InName);
        NameLength = Name.size();
    }

    const std::string& GetName() const {
        return Name;
    }

    static ClientMessageType GetMessageType() {
        return ClientMessageType::ConnectionRequest;
    }

    static std::vector<uint8_t> Serialize(const ConnectionRequestMessage& inMessage) {
        std::vector<uint8_t> bytes;
        bytes.push_back(static_cast<uint8_t>(GetMessageType()));

        for (auto i = 0; i < sizeof(size_t); ++i) {
            auto byte = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&inMessage.NameLength))[i];
            bytes.push_back(byte);
        }

        for (const char& c : inMessage.Name) {
            bytes.push_back(c);
        }
        return bytes;
    }

    static ConnectionRequestMessage Deserialize(asio::ip::tcp::socket& socket) {
        ConnectionRequestMessage message;

        asio::error_code ec;
        // the connection request has a length to determine the rest of the message
        socket.read_some(asio::buffer(&message.NameLength, sizeof(size_t)), ec);

        if (ec) {
            spdlog::error("Error reading message length: {}", ec.message());
        }

        message.Name.resize(message.NameLength);
        socket.read_some(asio::buffer(message.Name.data(), message.NameLength), ec);
        return message;
    }

    operator std::string() const {
        std::string str = "ConnectionRequestMessage: ";
        return str + Name;
    }

private:
    // Default constructor for deserialization
    ConnectionRequestMessage() = default;

    size_t NameLength {0};
    std::string Name;
};
