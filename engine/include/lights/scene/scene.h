//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include <lights/game/game_world.h>
#include <lights/input/input_subsystem.h>
#include <lights/scene/constants.h>
#include <lights/scene/scene_layer.h>
#include <lights/scene/scene_layer_manager.h>

namespace OZZ::scene {

    inline std::vector<SceneObject> operator+(std::vector<SceneObject> lhs, std::vector<SceneObject> rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }

    inline std::vector<SceneObject>& operator+=(std::vector<SceneObject>& lhs, const std::vector<SceneObject>& rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }

    class Scene {
    public:
        struct SceneParams {
            glm::vec3 ClearColor = {0.392f, 0.584f, 0.929f};
        };

        // Should be called at the end of the derived class Init function
        virtual void Init(std::shared_ptr<InputSubsystem> inInput) {
            input = std::move(inInput);
            layerManager = std::make_unique<SceneLayerManager>();

            const auto theWorld = GetWorld();
            theWorld->Init({
                .Gravity = {0.f, -20.f * game::constants::PhysicsUnitPerMeter},
            });
        }

        virtual void Tick(float DeltaTime) {
            static constexpr float physicsTickRate = 1.f / 60.f;
            static float accumulator = 0.f;
            accumulator += DeltaTime;

            const auto& activeLayers = GetActiveLayers();
            while (accumulator >= physicsTickRate) {
                for (const auto& Layer : activeLayers) {
                    Layer->PhysicsTick(physicsTickRate);
                }
                GetWorld()->PhysicsTick(physicsTickRate);
                accumulator -= physicsTickRate;
            }
            for (const auto& Layer : activeLayers) {
                Layer->Tick(DeltaTime);
            }
        };

        // Marked virtual to allow derived Scenes with custom entities
        virtual void WindowResized(const glm::ivec2 size) {
            // the render targets should resize even if they're not active so that when they become active again they're
            // correct
            for (const auto& Layer : GetAllLayers()) {
                Layer->RenderTargetResized(size);
            }
        }

        virtual ~Scene() {
            layerManager.reset();

            if (world)
                world->DeInit();
            world.reset();
        }

        [[nodiscard]] std::vector<SceneLayer*> GetActiveLayers() const { return layerManager->GetActiveLayers(); }

        [[nodiscard]] std::vector<SceneLayer*> GetAllLayers() const { return layerManager->GetAllLayers(); }

        GameWorld* GetWorld() {
            if (!world)
                world = std::make_shared<GameWorld>();
            return world.get();
        }

    public:
        SceneParams Params{};

    protected:
        std::unique_ptr<SceneLayerManager> layerManager;
        std::shared_ptr<InputSubsystem> input;

    private:
        std::shared_ptr<GameWorld> world;
    };
} // namespace OZZ::scene
