//
// Created by ozzadar on 2024-12-17.
//

#include "network/client.h"
#include "lights/network/client_messages.h"
#include "lights/network/server_messages.h"

using asio::ip::tcp;

namespace OZZ {
    Client::Client() : socket(context) {}

    void Client::Run(const std::string& host, short port) {
        if (OnConnectingToServer) {
            OnConnectingToServer();
        }
        context.restart();
        connect(host, port);
        read();
        context.run();
    }

    void Client::Stop() {
        context.stop();
        close();
    }

    void Client::Login(const std::string &username, const std::string &password) {
        LoginRequestMessage message(username, password);
        write(message);
    }

    void Client::Logout() {

    }

    void Client::connect(const std::string& host, short port) {
        socket = tcp::socket(context);
        tcp::resolver resolver(socket.get_executor());
        try {
            auto endpoint = asio::connect(socket, resolver.resolve(host, std::to_string(port)));
            spdlog::info("Connected to server: {}", endpoint.address().to_string());
            if (OnConnectedToServer) {
                OnConnectedToServer();
            }
        } catch (std::exception &e) {
            spdlog::error("Error connecting to server: {}", e.what());
            if (OnDisconnectedFromServer) {
                OnDisconnectedFromServer();
            }
            close();
        }
    }

    void Client::read() {
        socket.async_read_some(asio::buffer(&queuedMessageType, 1), [this](const asio::error_code &ec, size_t bytesTransferred) {
            handleRead(ec, bytesTransferred);
        });
    }

    void Client::handleRead(const asio::error_code &ec, size_t bytesTransferred) {
        if (ec == asio::error::eof || ec == asio::error::connection_reset) {
            spdlog::info("Connection closed by server");
            close();
            return;
        }

        if (bytesTransferred == 0) {
            return;
        }
        switch (queuedMessageType) {
            case ServerMessageType::AuthenticationFailed: {
                if (OnAuthenticationFailed) {
                    OnAuthenticationFailed();
                }
                return;
            }
            case ServerMessageType::UserLoggedIn: {
                auto receivedMessage = UserLoggedInMessage::Deserialize(socket);
                if (OnUserLoggedIn) {
                    OnUserLoggedIn(receivedMessage);
                }
                break;
            }
            case ServerMessageType::AccountLoggedInElsewhere: {
                if (OnAccountLoggedInElsewhere) {
                    OnAccountLoggedInElsewhere();
                }
                return;
            }
            default:
                spdlog::error("Unknown message type: {}", static_cast<uint8_t>(queuedMessageType));
        }

        read();
    }

    void Client::close() {
        asio::error_code ec;
        auto error = socket.close(ec);
        if (!ec && !error) {
            spdlog::info("Connection closed");
        } else {
            spdlog::error("Error closing connection: {}", ec.message());
        }

        if (OnDisconnectedFromServer) {
            OnDisconnectedFromServer();
        }
    }

}
