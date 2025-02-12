//
// Created by ozzadar on 2025-02-12.
//

#include "physics_testing_layer.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    PhysicsTestingLayer::PhysicsTestingLayer(World *inWorld) : world(inWorld) {}

    PhysicsTestingLayer::~PhysicsTestingLayer() {
        if (world) {
            world->RemoveObject(pepeid);
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [id, inpepe] = world->CreateGameObject<Pepe>();
        // scale pepe
        pepeid = id;
        pepe = static_cast<Pepe *>(inpepe);
        pepe->GetSceneObject()->Transform = glm::scale(glm::mat4{1.f}, glm::vec3(64.f, 64.f, 1.0f));
    }

    void PhysicsTestingLayer::Tick(float DeltaTime) {
        SceneLayer::Tick(DeltaTime);
    }

    void PhysicsTestingLayer::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Physics Resized to: {}x{}", size.x, size.y);
        auto width = size.x;
        auto height = size.y;

        LayerCamera.ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }

    std::vector<SceneObject> PhysicsTestingLayer::GetSceneObjects() {
        return {*pepe->GetSceneObject()};
    }
}
