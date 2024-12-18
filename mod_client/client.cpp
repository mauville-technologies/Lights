//
// Created by ozzadar on 2024-12-17.
//

#include "client.h"
#include "lights/network/client_messages.h"
#include "lights/network/server_messages.h"

using asio::ip::tcp;

namespace OZZ {
    Client::Client(asio::io_context &inContext, const std::string &host, short port) : socket(inContext) {
        connect(host, port);
    }

    void Client::Run() {
        write();
        read();
//        close();
    }

    void Client::connect(const std::string& host, short port) {
        tcp::resolver resolver(socket.get_executor());
        try {
            auto endpoint = asio::connect(socket, resolver.resolve(host, std::to_string(port)));
            spdlog::info("Connected to server: {}", endpoint.address().to_string());
        } catch (std::exception &e) {
            spdlog::error("Error connecting to server: {}", e.what());
        }
    }

    void Client::read() {
        socket.async_read_some(asio::buffer(&queuedMessageType, 1), [this](const asio::error_code &ec, size_t bytesTransferred) {
            handleRead(ec, bytesTransferred);
        });
    }

    void Client::handleRead(const asio::error_code &ec, size_t bytesTransferred) {
        if (bytesTransferred == 0) {
            return;
        }
        switch (queuedMessageType) {
            case ServerMessageType::ClientConnected: {
                auto receivedMessage = ClientConnectedMessage::Deserialize(socket);
                spdlog::info("Received message from server: {}", receivedMessage.GetMessage());
                break;
            }
            default:
                spdlog::error("Unknown message type: {}", static_cast<uint8_t>(queuedMessageType));
        }

        read();
    }

    void Client::write() {
        // Connection Request message
        ConnectionRequestMessage message("p.a.mauviel@gmail.com", "pass2word");
        asio::error_code ec;
        auto length = socket.write_some(asio::buffer(ConnectionRequestMessage::Serialize(message)), ec);
        if (!ec) {
            spdlog::info("Sent {} bytes to server: {}", length, std::string(message));
        } else {
            spdlog::error("Error writing to server: {}", ec.message());
        }
    }

    void Client::close() {
        asio::error_code ec;
        auto error = socket.close(ec);
        if (!ec && !error) {
            spdlog::info("Connection closed");
        } else {
            spdlog::error("Error closing connection: {}", ec.message());
        }
    }
}
