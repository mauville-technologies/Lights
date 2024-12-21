//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "game/scene/scene.h"
#include "game/scene/main_menu/objects/pepe.h"

namespace OZZ {

    class UILayer : public SceneLayer {
    public:
        void Init(std::shared_ptr<InputSubsystem> Input) override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;
    };

    class PepeLayer : public SceneLayer {
    public:
        PepeLayer();
        void Init(std::shared_ptr<InputSubsystem> Input) override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;

        void ChangeDirection();
    private:
        std::shared_ptr<Pepe> pepe { nullptr };
        glm::ivec2 movement { 0 };

        bool direction { false };
    };

    class MainMenuScene : public Scene {
    public:
        ~MainMenuScene() override = default;

        void Init(std::shared_ptr<InputSubsystem> Input) override;
        void Tick(float DeltaTime) override;
    private:

        std::shared_ptr<PepeLayer> pepeLayer;
        std::shared_ptr<PepeLayer> pepeLayer2;
    };

} // OZZ