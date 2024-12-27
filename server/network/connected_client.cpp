//
// Created by ozzadar on 2024-12-17.
//

#include "connected_client.h"

#include <utility>
#include "lights/network/server_messages.h"

namespace OZZ {
    ConnectedClient::ConnectedClient(std::shared_ptr<asio::ip::tcp::socket> Insocket)
        : socket(std::move(Insocket)), queuedMessageType(ClientMessageType::Unknown) {
        read();
//        ScheduleWrite();
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
                    {
                        std::lock_guard<std::mutex> lock(ClientMutex);
                        if (OnClose) {
                            OnClose();
                        }
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
                auto receivedMessage = LoginRequestMessage::Deserialize(*socket);
                {
                    std::lock_guard<std::mutex> lock(ClientMutex);
                    if (OnLoginRequest) {
                        OnLoginRequest(receivedMessage.GetEmail(), receivedMessage.GetPassword());
                    }
                }
                break;
            }
            default:
                spdlog::error("Unknown message type: {}", (int)queuedMessageType);
        }
        read();
    }

    void ConnectedClient::close() {
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

    void ConnectedClient::SendLoginResponse(bool success, const std::string& username) {
        if (!success) {
            write(AuthenticationFailedMessage());
            return;
        }
        auto ClientConnected = UserLoggedInMessage(username);
        write(ClientConnected);
    }

    void ConnectedClient::LoggedInElsewhere() {
        write(AccountLoggedInElsewhereMessage());
    }

} // OZZ