//
// Created by ozzadar on 2024-12-17.
//

#include "connected_client.h"

#include <utility>
#include "lights/network/server_messages.h"

namespace OZZ {
    ConnectedClient::ConnectedClient(std::shared_ptr<asio::ip::tcp::socket> Insocket, OnCloseHandler inCloseHandler)
        : socket(std::move(Insocket)), queuedMessageType(ClientMessageType::Unknown), closeHandler(std::move(inCloseHandler)), timer(socket->get_executor()) {
        read();
        ScheduleWrite();
    }

    ConnectedClient::~ConnectedClient() {
        close();
    }

    void ConnectedClient::read() {
        socket->async_read_some(asio::buffer(&queuedMessageType, 1), [this](std::error_code ec, std::size_t length) {
            if (!ec) {
                handleRead();
            } else {
                if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                    spdlog::info("Connection closed by client");
                    if (closeHandler) {
                        closeHandler(this);
                    }
                    return;
                }
                spdlog::error("Error reading message type from client: {}", ec.message());
            }
        });
    }

    void ConnectedClient::handleRead() {
        asio::error_code ec;

        switch (queuedMessageType) {
            case ClientMessageType::ConnectionRequest: {
                auto receivedMessage = ConnectionRequestMessage::Deserialize(*socket);
                spdlog::info("Received message: {}", std::string(receivedMessage));

                // Send connected message back
                std::string welcomeMessage = "Welcome to the server: ";
                welcomeMessage += receivedMessage.GetName();
                ClientConnectedMessage connectedMessage (welcomeMessage);
                auto outMessage = ClientConnectedMessage::Serialize(connectedMessage);
                socket->write_some(asio::buffer(outMessage), ec);
                break;
            }
            default:
                spdlog::error("Unknown message type: {}", (int)queuedMessageType);
        }
        read();
    }

    void ConnectedClient::write() {

    }

    void ConnectedClient::close() {
        // kill timer
        timer.cancel();

        if (!socket || !socket->is_open()) {
            spdlog::error("Socket is null or not open");
        } else {
            asio::error_code ec;
            // close the connection
            socket->close(ec);
            if (!ec) {
                spdlog::info("Connection closed");
            } else {
                spdlog::error("Error closing connection: {}", ec.message());
            }

        }
    }

    void ConnectedClient::ScheduleWrite() {
        timer.expires_after(std::chrono::seconds(5));
        timer.async_wait([this](std::error_code ec) {
            if (!ec) {
                spdlog::info("Timer expired");

                if (socket && socket->is_open()) {
                    // send a message to the client
                    ClientConnectedMessage connectedMessage("Server message");
                    auto outMessage = ClientConnectedMessage::Serialize(connectedMessage);
                    socket->write_some(asio::buffer(outMessage), ec);

                    if (ec) {
                        spdlog::error("Error sending message: {}", ec.message());
                    }
                }
                ScheduleWrite();
            } else {
                if (ec == asio::error::operation_aborted) {
                    spdlog::info("Timer cancelled");
                }  else {
                    spdlog::error("Error waiting for timer: {}", ec.message());
                }
            }
        });
    }

} // OZZ