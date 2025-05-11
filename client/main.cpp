//
// Created by ozzadar on 2024-12-17.
//

#include <lights/lights.h>
#include <spdlog/spdlog.h>
#include <game/scenes/physics_testing/physics_testing_scene.h>
#include <game/scenes/mmo/mmo_scene.h>
#include <lights/text/font_loader.h>

#include "game/client_game.h"

int main() {
    // Print library version
    spdlog::info("Lights version: {}.{}", (int)VERSION_MAJOR, (int)VERSION_MINOR);

     // OZZ::game::ClientGame<OZZ::game::scene::PhysicsTestingScene> game {};
    std::filesystem::path configFilePath = std::filesystem::current_path() / "clientConfig.toml";
    spdlog::info("Config file path: {}", configFilePath.string());
    OZZ::game::ClientGame<MMOScene> game {configFilePath};

    game.Run();

    return 0;
}