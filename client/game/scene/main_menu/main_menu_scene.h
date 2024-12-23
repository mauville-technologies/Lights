//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/scene/scene.h"
#include "game/scene/main_menu/objects/pepe.h"
#include "lights/ui/user_interface.h"
#include "game/ui/debug_window.h"

namespace OZZ {

    class UILayer : public SceneLayer {
    public:
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;
    };

    class PepeLayer : public SceneLayer {
    public:
        PepeLayer();
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;

        void ChangeDirection();
    private:
        void unregisterMappings(std::shared_ptr<InputSubsystem> inInput);
        void registerMappings(std::shared_ptr<InputSubsystem> inInput);
    private:
        std::shared_ptr<Pepe> pepe { nullptr };
        glm::ivec2 movement { 0 };

        bool direction { false };

        std::shared_ptr<InputSubsystem> input;
    };

    class MainMenuScene : public Scene {
    public:
        explicit MainMenuScene(std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI);
        ~MainMenuScene() override;

        void Init() override;
        void Tick(float DeltaTime) override;

        std::vector<std::shared_ptr<SceneLayer>>& GetLayers() override { return Layers; }
    private:
        std::vector<std::shared_ptr<SceneLayer>> Layers;
        std::shared_ptr<PepeLayer> pepeLayer;
        std::shared_ptr<PepeLayer> pepeLayer2;
        std::shared_ptr<InputSubsystem> input;

        std::shared_ptr<UserInterface> ui;
        std::shared_ptr<UIComponent> debugWindow;
    };

} // OZZ