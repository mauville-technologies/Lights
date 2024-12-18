//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <queue>
#include <asio.hpp>
#include "player.h"
#include "network/server.h"

namespace OZZ {

    class Game {
    public:
        Game(std::shared_ptr<Database> InDatabase);

        void Run();

    private:
        void tick();
        void startServer();

    private:
        std::mutex playersMutex;
        std::queue<Player*> playersToRemove;
        std::vector<std::shared_ptr<Player>> players;

        std::shared_ptr<Server> server;
        std::shared_ptr<Database> database;
        bool bRunning = false;
        asio::io_context context;
    };

} // OZZ