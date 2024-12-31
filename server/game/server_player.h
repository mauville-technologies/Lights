//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <memory>
#include "network/connected_client.h"
#include "database/database.h"

namespace OZZ::game {

    using OnPlayerLeftSignature = std::function<void(class ServerPlayer *)>;
    using OnPlayerLoggedInSignature = std::function<void(class ServerPlayer *)>;

    class ServerPlayer {
    public:
        explicit ServerPlayer(std::shared_ptr<network::server::ConnectedClient> Client, std::shared_ptr<Database> InDatabase);
        ~ServerPlayer();

        void LoggedInElsewhere();

        const std::string& GetEmail() const {
            return user.Email;
        };

    public:
        OnPlayerLeftSignature OnPlayerLeft;
        OnPlayerLoggedInSignature OnPlayerLoggedIn;
    private:
        std::shared_ptr<network::server::ConnectedClient> connectedClient;
        std::shared_ptr<Database> database;
        User user;
    };

} // OZZ