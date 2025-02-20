//
// Created by ozzadar on 2024-12-31.
//

#include <spdlog/spdlog.h>

#include "lights/game/game_world.h"

namespace OZZ {
    void GameWorld::Init(const WorldParams& params) {
        spdlog::info("Initializing world with gravity: ({}, {})", params.Gravity.x, params.Gravity.y);

        // Create collision system
        world = std::make_shared<OzzWorld2D>();
    }

    void GameWorld::PhysicsTick(float deltaTime) {
        world->PhysicsTick(deltaTime);
    }

    void GameWorld::Tick(float deltaTime) {

    }

    GameObject *GameWorld::GetObject(uint64_t id) {
        if (objects.contains(id)) {
            return objects[id].get();
        }
        return nullptr;
    }

    void GameWorld::RemoveObject(uint64_t id) {
        spdlog::info("Objects before delete: {}", objects.size());
        if (objects.erase(id) == 1) {
            spdlog::info("erased object");
        }
        spdlog::info("Objects after delete: {}", objects.size());
    }

    void GameWorld::DeInit() {
        objects.clear();

    }
} // OZZ