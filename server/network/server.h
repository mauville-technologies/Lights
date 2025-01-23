//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <thread>

#include "lights/network/messages.h"
#include "connected_client.h"

namespace OZZ::network::server {
    using asio::ip::tcp;

    using OnNewClientSignature = std::function<void(std::shared_ptr<ConnectedClient>)>;

    class Server {
    public:
        Server(asio::io_context& inContext, short inPort);
        ~Server();
    private:
        void startAccept();

    public:
        OnNewClientSignature OnNewClient;

    private:
        asio::io_context& context;
        asio::executor_work_guard<asio::io_context::executor_type> workGuard;

        tcp::acceptor acceptor;
        std::vector<std::shared_ptr<ConnectedClient>> clients;
        std::vector<std::thread> threadPool;
    };

} // OZZ
