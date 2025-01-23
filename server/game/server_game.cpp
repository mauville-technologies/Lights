//
// Created by ozzadar on 2024-12-17.
//

#include <chrono>
#include "server_game.h"
#include "network/server.h"
#include "server_player.h"

namespace OZZ::game {
    ServerGame::ServerGame(std::shared_ptr<Database> InDatabase) : database(std::move(InDatabase)) {

    }

    void ServerGame::Run() {
        startServer();

        // use high precision clock to tick at 30 FPS
        auto lastTickTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto tickRate = std::chrono::duration<float>(1.0f / 1.0f);

        while (bRunning) {
            currentTime = std::chrono::high_resolution_clock::now();
            auto deltaTime = currentTime - lastTickTime;

            if (deltaTime >= tickRate) {
                lastTickTime = currentTime;
                // tick
                tick();
            }
        }
    }

    void ServerGame::tick() {
        // drop any players that have left
        {
            std::lock_guard<std::mutex> lock(playersMutex);
            while (!playersToRemove.empty()) {
                auto player = playersToRemove.front();
                playersToRemove.pop();

                std::erase_if (players, [player](const std::shared_ptr<ServerPlayer>& p) {
                    return p.get() == player;
                });

                spdlog::info("Player left, connected player count: {}", players.size());
            }
        }
    }

    void ServerGame::startServer() {
        try {
            server = std::make_shared<network::server::Server>(context, 1337);
            server->OnNewClient = [this](const std::shared_ptr<OZZ::network::server::ConnectedClient> &client) {
                std::lock_guard<std::mutex> lock(playersMutex);
                spdlog::info("New client connected");

                // create a new player object and add it to the list
                std::shared_ptr<ServerPlayer> newPlayer;
                {
                    newPlayer = players.emplace_back(std::make_shared<ServerPlayer>(client, database));
                }

                newPlayer->OnPlayerLeft = [this](ServerPlayer *lostPlayer) {
                    playersToRemove.emplace(lostPlayer);
                };

                newPlayer->OnPlayerLoggedIn = [this](ServerPlayer *loggedInPlayer) {
                    spdlog::info("Player logged in: {}", loggedInPlayer->GetEmail());

                    // the same player logged in again, disconnect the previous client
                    for (const auto& player : players) {
                        if (player.get() != loggedInPlayer && player->GetEmail() == loggedInPlayer->GetEmail()) {
                            player->LoggedInElsewhere();
                            playersToRemove.emplace(player.get());
                            break;
                        }
                    }
                };

                spdlog::info("Connected Player Count: {}", players.size());
            };
            bRunning = true;
        } catch (std::exception &e) {
            spdlog::error("Exception: {}", e.what());
            bRunning = false;
        }
    }
} // OZZ