//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <memory>
#include "network/connected_client.h"
#include "database/database.h"

namespace OZZ {

    using OnPlayerLeftSignature = std::function<void(class Player *)>;

    class Player {
    public:
        explicit Player(std::shared_ptr<ConnectedClient> Client, std::shared_ptr<Database> InDatabase);
        ~Player();
    public:
        OnPlayerLeftSignature OnPlayerLeft;
    private:
        std::shared_ptr<ConnectedClient> connectedClient;
        std::shared_ptr<Database> database;
    };

} // OZZ