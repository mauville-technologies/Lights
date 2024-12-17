//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <asio.hpp>
#include <thread>

#include "lights/network/client_messages.h"
#include "connected_client.h"

namespace OZZ {
    using asio::ip::tcp;

    class Server {
    public:
        Server(asio::io_context& inContext, short inPort);
        ~Server();
    private:
        void startAccept();

        void clientDisconnected(ConnectedClient* client);

    private:
        asio::io_context& context;
        tcp::acceptor acceptor;
        std::vector<std::shared_ptr<ConnectedClient>> clients;
        std::vector<std::thread> threadPool;
    };

} // OZZ
