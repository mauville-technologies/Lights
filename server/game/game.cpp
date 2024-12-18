//
// Created by ozzadar on 2024-12-17.
//

#include <chrono>
#include "game.h"
#include "network/server.h"
#include "player.h"

namespace OZZ {
    Game::Game(std::shared_ptr<Database> InDatabase) : database(std::move(InDatabase)) {

    }

    void Game::Run() {
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

    void Game::tick() {
        // drop any players that have left
        {
            std::lock_guard<std::mutex> lock(playersMutex);
            while (!playersToRemove.empty()) {
                auto player = playersToRemove.front();
                playersToRemove.pop();

                std::erase_if (players, [player](const std::shared_ptr<Player>& p) {
                    return p.get() == player;
                });

                spdlog::info("Player left, connected player count: {}", players.size());
            }
        }
    }

    void Game::startServer() {
        try {
            server = std::make_shared<Server>(context, 8080);
            server->OnNewClient = [this](const std::shared_ptr<OZZ::ConnectedClient> &client) {
                std::lock_guard<std::mutex> lock(playersMutex);
                spdlog::info("New client connected");

                // create a new player object and add it to the list
                std::shared_ptr<Player> newPlayer;
                {
                    newPlayer = players.emplace_back(std::make_shared<Player>(client, database));
                }

                newPlayer->OnPlayerLeft = [this](Player *lostPlayer) {
                    playersToRemove.emplace(lostPlayer);
                };

                newPlayer->OnPlayerLoggedIn = [this](Player *loggedInPlayer) {
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