//
// Created by ozzadar on 2024-12-18.
//

#include "main_menu_scene.h"
#include "game/scene/main_menu/objects/pepe.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "spdlog/spdlog.h"

namespace OZZ {

    void MainMenuScene::Init() {
        camera = std::make_shared<Camera>();

        camera->ViewMatrix = glm::lookAt(glm::vec3(0.f, 0.f, 3.f),  // Camera position
                                         glm::vec3(0.f, 0.f, 0.f),  // Target to look at
                                         glm::vec3(0.f, 1.f, 0.f)); // Up vector

        // Create a pepe
        pepe = std::make_shared<Pepe>();
        objects.push_back(pepe);

        // scale da pepe
        pepe->Transform = glm::scale(pepe->Transform, glm::vec3(64.f, 64.f, 1.0f));
    }

    void MainMenuScene::Tick(float DeltaTime) {

    }

    const std::vector<std::shared_ptr<SceneObject>> &MainMenuScene::GetObjects() const {
        return objects;
    }

    const std::vector<std::shared_ptr<LightSource>> &MainMenuScene::GetLights() const {
        return lights;
    }

    std::shared_ptr<Camera> MainMenuScene::GetCamera() const {
        return camera;
    }

    void MainMenuScene::RenderTargetResized(glm::ivec2 size) {
        spdlog::info("Main Menu Scene Resized to: {}x{}", size.x, size.y);
        auto width = size.x;
        auto height = size.y;

        camera->ProjectionMatrix = glm::ortho(-width / 2.f, width / 2.f, -height / 2.f, height / 2.f, 0.1f, 100.f);
    }
} // OZZ