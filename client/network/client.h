//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <spdlog/spdlog.h>
#include "lights/network/server_messages.h"

namespace OZZ {
    class Client {
    public:
        using OnAuthenticationFailedCallback = std::function<void()>;
        using OnClientConnectedCallback = std::function<void(ClientConnectedMessage)>;
        using OnAccountLoggedInElsewhereCallback = std::function<void()>;
        using OnUnknownMessageCallback = std::function<void(ServerMessageType)>;

        Client();

        void Run(const std::string& host, short port);
        void Stop();

        OnAuthenticationFailedCallback OnAuthenticationFailed;
        OnClientConnectedCallback OnClientConnected;
        OnAccountLoggedInElsewhereCallback OnAccountLoggedInElsewhere;
        OnUnknownMessageCallback OnUnknownMessage;

    private:
        void connect(const std::string& host, short port);
        void read();
        void handleRead(const asio::error_code& ec, size_t bytesTransferred);
        void write();
        void close();

    private:
        asio::io_context context;
        asio::ip::tcp::socket socket;
        ServerMessageType queuedMessageType;
    };
} // OZZ