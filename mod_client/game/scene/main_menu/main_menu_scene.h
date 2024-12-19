//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "game/scene/scene.h"
namespace OZZ {

    class MainMenuScene : public Scene {
    public:
        void Tick(float DeltaTime) override;

        const std::vector<std::shared_ptr<SceneObject>> &GetObjects() const override;

        const std::vector<std::shared_ptr<LightSource>> &GetLights() const override;

        std::shared_ptr<Camera> GetCamera() const override;

        ~MainMenuScene() override = default;

    public:
    };

} // OZZ