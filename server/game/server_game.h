//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <queue>
#include <asio.hpp>
#include "server_player.h"
#include "network/server.h"

namespace OZZ::game {

    class ServerGame {
    public:
        ServerGame(std::shared_ptr<Database> InDatabase);

        void Run();

    private:
        void tick();
        void startServer();

    private:
        std::mutex playersMutex;
        std::queue<ServerPlayer*> playersToRemove;
        std::vector<std::shared_ptr<ServerPlayer>> players;

        std::shared_ptr<network::server::Server> server;
        std::shared_ptr<Database> database;
        bool bRunning = false;
        asio::io_context context;
    };

} // OZZ