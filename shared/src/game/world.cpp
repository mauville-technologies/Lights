//
// Created by ozzadar on 2024-12-31.
//

#include <spdlog/spdlog.h>

#include "lights/game/world.h"

namespace OZZ {
    void World::Init(const WorldParams& params) {
        spdlog::info("Initializing world with gravity: ({}, {})", params.Gravity.x, params.Gravity.y);
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = b2Vec2(params.Gravity.x, params.Gravity.y);
        worldId = b2CreateWorld(&worldDef);
    }

    void World::PhysicsTick(float deltaTime) {
        if (worldId.index1 == 0) {
            spdlog::error("Cannot tick physics without a valid world");
            return;
        }

        static constexpr uint8_t subSteps = 4;

        b2World_Step(worldId, deltaTime, subSteps);
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

        if (worldId.index1 != 0) {
            spdlog::info("Destroying physics world");
            b2DestroyWorld(worldId);
            worldId = {0, 0};
        }
    }
} // OZZ