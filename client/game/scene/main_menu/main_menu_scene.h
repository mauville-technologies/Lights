//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/scene/scene.h"
#include "game/scene/main_menu/objects/pepe.h"
#include "lights/ui/user_interface.h"
#include "game/ui/debug_window.h"
#include "game/application_state.h"
#include "game/callback_functions.h"
#include "lights/game/world.h"
#include "game/scene/main_menu/objects/ground_test.h"

namespace OZZ::game::scene {

    class UILayer : public SceneLayer {
    public:
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;
    };

    class PepeLayer : public SceneLayer {
    public:
        PepeLayer(World* inWorld);
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;

        void ChangeDirection();
    private:
        void unregisterMappings(std::shared_ptr<InputSubsystem> inInput);
        void registerMappings(std::shared_ptr<InputSubsystem> inInput);
    private:
        Pepe* pepe { nullptr };
        GroundTest* ground { nullptr };
        glm::ivec2 movement { 0 };

        bool direction { false };

        std::shared_ptr<InputSubsystem> input;

        World* world;
    };

    class MainMenuScene : public Scene {
    public:
        explicit MainMenuScene(GetApplicationStateFunction inAppStateFunction, std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI);
        ~MainMenuScene() override;

        void Init() override;
        void Tick(float DeltaTime) override;

        std::vector<std::shared_ptr<SceneLayer>>& GetLayers() override { return Layers; }

        ConnectToServerRequestedFunction ConnectToServerRequested;
        DisconnectFromServerRequestedFunction DisconnectFromServerRequested;
        LoginRequestedFunction LoginRequested;
        LogoutRequestedFunction LogoutRequested;

    private:
        GetApplicationStateFunction appStateFunction;
        std::vector<std::shared_ptr<SceneLayer>> Layers;
        std::shared_ptr<PepeLayer> pepeLayer;
        std::shared_ptr<InputSubsystem> input;

        std::shared_ptr<UserInterface> ui;
        std::shared_ptr<UIComponent> debugWindow;
        std::shared_ptr<World> world;
    };

} // OZZ