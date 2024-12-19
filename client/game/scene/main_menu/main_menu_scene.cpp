//
// Created by ozzadar on 2024-12-18.
//

#include "main_menu_scene.h"

namespace OZZ {

    void MainMenuScene::Tick(float DeltaTime) {

    }

    const std::vector<std::shared_ptr<SceneObject>> &MainMenuScene::GetObjects() const {
        return {};
    }

    const std::vector<std::shared_ptr<LightSource>> &MainMenuScene::GetLights() const {
        return {};
    }

    std::shared_ptr<Camera> MainMenuScene::GetCamera() const {
        return nullptr;
    }
} // OZZ