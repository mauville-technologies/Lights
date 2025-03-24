//
// Created by ozzadar on 2024-12-31.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <random>
#include <limits>
#include <spdlog/spdlog.h>

#include "lights/game/game_object.h"

/**
 * The World class manages the entire game state. It is the M of MVC. The client is the C and the server is the V.
 * In theory however, everything could be done locally in the case of a single player game.
 */
namespace OZZ {
    struct WorldParams {
        glm::vec2 Gravity = {0.f, -9.8f};
    };

    class GameWorld {
    public:
        GameWorld() = default;
        ~GameWorld() = default;

        void Init(const WorldParams& params = {});

        void PhysicsTick(float deltaTime);
        void Tick(float deltaTime);

        void DeInit();

        inline auto& GetObjects() {
            return objects;
        }

        template <typename T, typename... Args>
        std::pair<uint64_t, T*> CreateGameObject(Args&&... inArgs) {
            auto id = generateUnusedID();
            auto newObject = std::make_unique<T>(this, world, std::forward<Args>(inArgs)...);
            objects[id] = std::move(newObject);
            return {id, dynamic_cast<T*>(objects[id].get())};
        }

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

        std::shared_ptr<OzzWorld2D> world;
    };
}