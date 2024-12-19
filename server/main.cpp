//
// Created by ozzadar on 2024-12-17.
//

#include <lights/lights.h>
#include <spdlog/spdlog.h>
#include <asio.hpp>

#include "database/database.h"
#include "game/server_game.h"

int main() {
    spdlog::info("Lights version: {}.{}", (int)VERSION_MAJOR, (int)VERSION_MINOR);

    /**
     * Database Testing
     */

    auto db = std::make_shared<OZZ::Database>();

    OZZ::ServerGame game {db};
    game.Run();

    return 0;
}