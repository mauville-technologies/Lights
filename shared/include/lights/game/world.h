//
// Created by ozzadar on 2024-12-31.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <random>
#include <limits>

#include "lights/game/game_object.h"

/**
 * The World class manages the entire game state. It is the M of MVC. The client is the C and the server is the V.
 * In theory however, everything could be done locally in the case of a single player game.
 */
namespace OZZ {
    class World {
    public:
        World() = default;
        ~World() = default;

        inline auto& GetObjects() {
            return objects;
        }

        uint64_t AddObject(std::unique_ptr<GameObject> object);
        GameObject* GetObject(uint64_t id);
        void RemoveObject(uint64_t id);

    private:
        uint64_t generateUnusedID() {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

            uint64_t id = dis(gen);

            // We should make sure that the ID is unique
            while(id == 0 || objects.contains(id)) {
                id = dis(gen);
            }

            return id;
        }

    private:
        // TODO: things should be reference by ID, which uint64_t might not be the best choice
        std::unordered_map<uint64_t, std::unique_ptr<GameObject>> objects;
    };
}