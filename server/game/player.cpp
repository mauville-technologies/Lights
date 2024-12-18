//
// Created by ozzadar on 2024-12-17.
//

#include "player.h"

#include <utility>

namespace OZZ {
    Player::Player(std::shared_ptr<ConnectedClient> ConnectedClient, std::shared_ptr<Database> InDatabase) : connectedClient(std::move(ConnectedClient)), database(std::move(InDatabase)) {
        connectedClient->OnClose = [this]() {
            OnPlayerLeft(this);
        };

        connectedClient->OnLoginRequest = [this](const std::string& Email, const std::string& Password) {
            spdlog::info("Player::OnLoginRequest: Email: {}, Password: {}", Email, Password);
            if (database){
                auto loggedIn = database->LoginUser(Email, Password);

                connectedClient->SendLoginResponse((bool)loggedIn);
                if (!loggedIn) {
                    OnPlayerLeft(this);
                }
            }
        };
    }

    Player::~Player() {
        connectedClient->OnClose = nullptr;
        connectedClient.reset();
    }


} // OZZ