//
// Created by ozzadar on 2025-09-01.
//

#include "lights/library/layers/world_2d_layer.h"
#include "lights/game/game_world.h"

namespace OZZ::lights {
    World2DLayer::World2DLayer(OZZ::GameWorld* inWorld, const std::shared_ptr<OZZ::InputSubsystem>& inInput)
        : gameWorld(inWorld), input(inInput) {}

    void World2DLayer::Init() {
        SceneLayer::Init();
    }

    void World2DLayer::PhysicsTick(float DeltaTime) {
        SceneLayer::PhysicsTick(DeltaTime);
    }

    void World2DLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);

        // TODO: I feel like the gameworld doesn't need to be owned by the layer -- though I'm not entirely sure of
        // the alternative. I'll leave it here for now and see if it makes sense to move it somewhere else (like the
        // scene) further down the line Doing it here makes it easy to forget in the future if
        gameWorld->Tick(DeltaTime);
        LookAtPosition2D(cameraPosition);
    }

    void World2DLayer::RenderTargetResized(glm::ivec2 size) {
        const auto width = size.x * 2;
        const auto height = size.y * 2;

        LayerCamera.ProjectionMatrix =
            glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.001f, 1000.f);
        viewportSize = size;
    }
} // namespace OZZ::lights