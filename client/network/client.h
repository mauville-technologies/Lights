//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include "lights/network/messages.h"

namespace OZZ::network::client {
    class Client {
    public:
        using OnConnectedToServerCallback = std::function<void()>;
        using OnConnectingToServerCallback = std::function<void()>;
        using OnDisconnectedFromServerCallback = std::function<void()>;
        using OnAuthenticationFailedCallback = std::function<void()>;
        using OnUserLoggedInCallback = std::function<void(network::messages::server::AuthenticationSuccessful)>;
        using OnLoggedInElsewhereCallback = std::function<void()>;
        using OnLoggedOutCallback = std::function<void()>;
        using OnUnknownMessageCallback = std::function<void(network::messages::ServerMessageType)>;

        Client();

        void Run(const std::string& host, short port);
        void Stop();

        void Login(const std::string& username, const std::string& password);
        void Logout();

        OnConnectedToServerCallback OnConnectedToServer;
        OnConnectingToServerCallback OnConnectingToServer;
        OnDisconnectedFromServerCallback OnDisconnectedFromServer;
        OnAuthenticationFailedCallback OnAuthenticationFailed;
        OnUserLoggedInCallback OnUserLoggedIn;
        OnLoggedInElsewhereCallback OnAccountLoggedInElsewhere;
        OnLoggedOutCallback OnLoggedOut;
        OnUnknownMessageCallback OnUnknownMessage;

    private:
        void connect(const std::string& host, short port);
        void read();
        void handleRead(const asio::error_code& ec, size_t bytesTransferred);
        void close();

        template<typename PayloadType>
        void write(PayloadType& Payload) {
            asio::error_code ec;
            auto length = socket.write_some(asio::buffer(std::vector<uint8_t>(Payload)), ec);
            if (!ec) {
                spdlog::info("Sent {} bytes to server: {}", length, std::string(Payload));
            } else {
                spdlog::error("Error writing to server: {}", ec.message());
            }
        }

    private:
        std::mutex socketMutex;
        asio::io_context context;
        asio::ip::tcp::socket socket;
        network::messages::ServerMessageType queuedMessageType;
    };
} // OZZ