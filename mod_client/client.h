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
        Client(asio::io_context& inContext, const std::string& host, short port);

        void Run();
    private:
        void connect(const std::string& host, short port);
        void read();
        void handleRead(const asio::error_code& ec, size_t bytesTransferred);
        void write();
        void close();

    private:
        asio::ip::tcp::socket socket;
        ServerMessageType queuedMessageType;
    };
} // OZZ