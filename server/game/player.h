//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <memory>
#include "network/connected_client.h"
#include "database/database.h"

namespace OZZ {

    using OnPlayerLeftSignature = std::function<void(class Player *)>;
    using OnPlayerLoggedInSignature = std::function<void(class Player *)>;

    class Player {
    public:
        explicit Player(std::shared_ptr<ConnectedClient> Client, std::shared_ptr<Database> InDatabase);
        ~Player();

        void LoggedInElsewhere();

        const std::string& GetEmail() const {
            return user.Email;
        };

    public:
        OnPlayerLeftSignature OnPlayerLeft;
        OnPlayerLoggedInSignature OnPlayerLoggedIn;
    private:
        std::shared_ptr<ConnectedClient> connectedClient;
        std::shared_ptr<Database> database;
        User user;
    };

} // OZZ