//
// Created by ozzadar on 2024-12-31.
//

#include "lights/game/world.h"

namespace OZZ {
    uint64_t World::AddObject(std::unique_ptr<GameObject> object) {
        auto id = generateUnusedID();
        objects[id] = std::move(object);
        return id;
    }

    GameObject *World::GetObject(uint64_t id) {
        if (objects.contains(id)) {
            return objects[id].get();
        }
        return nullptr;
    }

    void World::RemoveObject(uint64_t id) {
        objects.erase(id);
    }
} // OZZ