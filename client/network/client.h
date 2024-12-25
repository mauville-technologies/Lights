//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include "lights/network/server_messages.h"

namespace OZZ {
    class Client {
    public:
        using OnConnectedToServerCallback = std::function<void()>;
        using OnConnectingToServerCallback = std::function<void()>;
        using OnDisconnectedFromServerCallback = std::function<void()>;
        using OnAuthenticationFailedCallback = std::function<void()>;
        using OnUserLoggedInCallback = std::function<void(UserLoggedInMessage)>;
        using OnLoggedInElsewhereCallback = std::function<void()>;
        using OnLoggedOutCallback = std::function<void()>;
        using OnUnknownMessageCallback = std::function<void(ServerMessageType)>;

        Client();

        void Run(const std::string& host, short port);
        void Stop();

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
        void write();
        void close();

    private:
        std::mutex socketMutex;
        asio::io_context context;
        asio::ip::tcp::socket socket;
        ServerMessageType queuedMessageType;
    };
} // OZZ