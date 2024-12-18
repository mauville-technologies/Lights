//
// Created by ozzadar on 2024-12-17.
//

#include "player.h"

#include <utility>

namespace OZZ {
    Player::Player(std::shared_ptr<ConnectedClient> ConnectedClient, std::shared_ptr<Database> InDatabase) : connectedClient(std::move(ConnectedClient)), database(std::move(InDatabase)) {
        connectedClient->OnClose = [this]() {
            database->LogoutUser(user.Email);
            OnPlayerLeft(this);
        };

        connectedClient->OnLoginRequest = [this](const std::string& Email, const std::string& Password) {
            // check to see if user is already connected. If they are, disconnect the currently connected user.
            if (database){
                auto loggedIn = database->LoginUser(Email, Password);

                connectedClient->SendLoginResponse((bool)loggedIn);
                if (!loggedIn) {
                    return;
                }

                user = loggedIn.value();
                OnPlayerLoggedIn(this);
            }
        };
    }

    Player::~Player() {
        connectedClient->OnClose = nullptr;
        connectedClient.reset();
    }

    void Player::LoggedInElsewhere() {
        connectedClient->LoggedInElsewhere();
    }


} // OZZ