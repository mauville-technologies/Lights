//
// Created by ozzadar on 2024-12-31.
//

#include <spdlog/spdlog.h>

#include "lights/game/world.h"

namespace OZZ {
    void World::Init(const WorldParams& params) {
        spdlog::info("Initializing world with gravity: ({}, {})", params.Gravity.x, params.Gravity.y);

        // Create collision system
        collisionSystem = std::make_shared<CollisionSystem>();
    }

    void World::PhysicsTick(float deltaTime) {

    }

    void World::Tick(float deltaTime) {

    }

    GameObject *World::GetObject(uint64_t id) {
        if (objects.contains(id)) {
            return objects[id].get();
        }
        return nullptr;
    }

    void World::RemoveObject(uint64_t id) {
        spdlog::info("Objects before delete: {}", objects.size());
        if (objects.erase(id) == 1) {
            spdlog::info("erased object");
        }
        spdlog::info("Objects after delete: {}", objects.size());
    }

    void World::DeInit() {
        objects.clear();

    }
} // OZZ