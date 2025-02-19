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
            world->RemoveObject(pepe2id);
        }
    }

    void PhysicsTestingLayer::Init() {
        LayerCamera.ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        auto [id, inpepe] = world->CreateGameObject<Sprite>("assets/textures/pepe.png");
        // scale pepe
        pepeid = id;
        pepe = static_cast<Sprite *>(inpepe);
        pepe->Scale *= 1.f;

        auto [id2, inpepe2] = world->CreateGameObject<Sprite>("assets/textures/pepe.png");
        pepe2 = static_cast<Sprite *>(inpepe2);
        pepe2id = id2;
        pepe2->Scale *= 1.f;
        pepe2->Position = {0.f, 1.f, 0.f};
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
        auto pepeSceneObject = pepe->GetSceneObjects();
        auto pepe2SceneObject = pepe2->GetSceneObjects();

        std::vector<SceneObject> sceneObjects;
        sceneObjects.reserve(pepeSceneObject.size() + pepe2SceneObject.size());
        sceneObjects.insert(sceneObjects.end(), pepeSceneObject.begin(), pepeSceneObject.end());
        sceneObjects.insert(sceneObjects.end(), pepe2SceneObject.begin(), pepe2SceneObject.end());
        return sceneObjects;
    }
}
