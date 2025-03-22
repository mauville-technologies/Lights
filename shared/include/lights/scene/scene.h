//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <lights/game/game_world.h>
#include <lights/input/input_subsystem.h>
#include <lights/ui/user_interface.h>
#include "lights/scene/constants.h"
#include <lights/scene/scene_layer.h>


namespace OZZ::scene {

    inline std::vector<SceneObject>& operator+(std::vector<SceneObject> &lhs, std::vector<SceneObject> &rhs) {
        lhs.insert(lhs.end(), rhs.begin(), rhs.end());
        return lhs;
    }


    class Scene {
    public:
        // Should be called at the end of the derived class Init function
        virtual void Init(std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI) {
            ui = std::move(inUI);
            input = std::move(inInput);

            const auto theWorld = GetWorld();
            theWorld->Init({
                .Gravity = {0.f, -20.f * game::constants::PhysicsUnitPerMeter},
            });

            for (const auto &Layer: Layers) {
                Layer->Init();
            }
        }

        virtual void Tick(float DeltaTime) {
            static constexpr float physicsTickRate = 1.f / 60.f;
            static float accumulator = 0.f;
            accumulator += DeltaTime;

            while (accumulator >= physicsTickRate) {
                for (const auto &Layer: GetLayers()) {
                    // Layer->PhysicsTick(physicsTickRate);
                }
                GetWorld()->PhysicsTick(physicsTickRate);
                accumulator -= physicsTickRate;
            }
            for (const auto &Layer: GetLayers()) {
                Layer->Tick(DeltaTime);
            }
        };

        // Marked virtual to allow derived Scenes with custom entities
        void RenderTargetResized(glm::ivec2 size) {
            for (auto &Layer: GetLayers()) {
                Layer->RenderTargetResized(size);
            }
        }

        virtual ~Scene() {
            Layers.clear();

            if (world) world->DeInit();
            world.reset();
        }

        std::vector<std::shared_ptr<SceneLayer> > &GetLayers() { return Layers; }

        GameWorld *GetWorld() {
            if (!world) world = std::make_shared<GameWorld>();
            return world.get();
        }

    protected:
        std::vector<std::shared_ptr<SceneLayer> > Layers;
        std::shared_ptr<UserInterface> ui;
        std::shared_ptr<InputSubsystem> input;

    private:
        std::shared_ptr<GameWorld> world;
    };
}
