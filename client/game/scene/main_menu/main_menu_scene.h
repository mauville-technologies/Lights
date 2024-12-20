//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "game/scene/scene.h"
#include "game/scene/main_menu/objects/pepe.h"

namespace OZZ {

    class MainMenuScene : public Scene {
    public:
        ~MainMenuScene() override = default;

        void Init() override;
        void Tick(float DeltaTime) override;

        [[nodiscard]] const std::vector<std::shared_ptr<SceneObject>> &GetObjects() const override;
        [[nodiscard]] const std::vector<std::shared_ptr<LightSource>> &GetLights() const override;
        [[nodiscard]] std::shared_ptr<Camera> GetCamera() const override;

        void RenderTargetResized(glm::ivec2 size) override;

    private:
        std::shared_ptr<Pepe> pepe;
        std::vector<std::shared_ptr<SceneObject>> objects {};
        std::vector<std::shared_ptr<LightSource>> lights {};
        std::shared_ptr<Camera> camera { nullptr };
    };

} // OZZ