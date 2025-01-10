//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/game/world.h"
#include "lights/scene/scene.h"
#include "lights/ui/user_interface.h"

#include "game/callback_functions.h"
#include "game/scene/base_scene/objects/pepe.h"
#include "game/scene/base_scene/objects/ground_test.h"
#include "game/scene/base_scene/objects/tilemap/tilemap.h"

namespace OZZ::game::scene {

    class GameLayer : public SceneLayer {
    public:
        explicit GameLayer(World* inWorld);
        ~GameLayer() override;
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;

        std::vector<SceneObject> GetSceneObjects() override;
    private:
        void unregisterMappings(std::shared_ptr<InputSubsystem> inInput);
        void registerMappings(std::shared_ptr<InputSubsystem> inInput);
    private:
        std::pair<uint64_t, Pepe*> pepe {0, nullptr};
        std::pair<uint64_t, GroundTest*> ground {0, nullptr};
        std::pair<uint64_t, Tilemap*> tilemap {0, nullptr};

        std::shared_ptr<InputSubsystem> input;

        World* world;
    };

    class BaseScene : public Scene {
    public:
        explicit BaseScene(GetApplicationStateFunction inAppStateFunction, std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI);
        ~BaseScene() override;

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
        std::shared_ptr<GameLayer> pepeLayer;
        std::shared_ptr<InputSubsystem> input;

        std::shared_ptr<UserInterface> ui;
        std::shared_ptr<UIComponent> debugWindow;
        std::shared_ptr<World> world;
    };

} // OZZ