//
// Created by ozzadar on 2025-02-12.
//

#include "physics_testing_scene.h"

#include "layers/physics_testing_layer.h"

namespace OZZ::game::scene {
    void PhysicsTestingScene::Init(std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI) {
        mainLayer = std::make_shared<PhysicsTestingLayer>(GetWorld());
        mainLayer->SetInputSubsystem(inInput);
        Layers.emplace_back(mainLayer);

        // Should be done at the bottom
        Scene::Init(inInput, inUI);
    }
}
