//
// Created by ozzadar on 2024-12-31.
//

#include <spdlog/spdlog.h>
#include <ranges>
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
        // Tick all game objects
        for (const auto& object : objects | std::views::values) {
            object->Tick(deltaTime);
        }
    }

    GameObject *GameWorld::GetObject(uint64_t id) {
        if (objects.contains(id)) {
            return objects[id].get();
        }
        return nullptr;
    }

    void GameWorld::RemoveObject(uint64_t id) {
        if (objects.erase(id) != 1) {
            spdlog::info("Failed to remove object with id: {}", id);
        }
    }

    void GameWorld::DeInit() {
        objects.clear();
    }
} // OZZ