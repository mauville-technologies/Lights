//
// Created by ozzadar on 2025-09-01.
//

#pragma once
#include "lights/scene/scene_layer.h"

namespace OZZ {
    class InputSubsystem;
    class GameWorld;

    namespace lights {
        class World2DLayer : public OZZ::scene::SceneLayer {
        public:
            explicit World2DLayer(OZZ::GameWorld *inWorld, const std::shared_ptr<OZZ::InputSubsystem> &inInput);
            ~World2DLayer() override = default;
            void Init() override;
            void PhysicsTick(float DeltaTime) override;
            void Tick(float DeltaTime) override;
            void RenderTargetResized(glm::ivec2 size) override;

        protected:
            OZZ::GameWorld *gameWorld{nullptr};
            std::shared_ptr<OZZ::InputSubsystem> input{nullptr};
            std::vector<OZZ::scene::SceneObject> sceneObjects{};

            glm::vec2 cameraPosition{0.f, 0.f};
        };
    } // namespace lights
} // namespace OZZ
