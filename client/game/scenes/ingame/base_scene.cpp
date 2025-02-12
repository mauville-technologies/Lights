//
// Created by ozzadar on 2024-12-18.
//

#include "base_scene.h"

#include <utility>
#include "game/ui/debug_window.h"
#include "layers/game_layer.h"

namespace OZZ::game::scene {

    BaseScene::BaseScene(GetApplicationStateFunction inAppStateFunction)
        : appStateFunction(std::move(inAppStateFunction)) {}

    BaseScene::~BaseScene() {
        input.reset();

        debugWindow.reset();
        ui.reset();

    }

    void BaseScene::Init(std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI) {
        debugWindow = std::make_shared<ui::DebugWindow>(appStateFunction);

        if (auto typedDebugWindow = std::static_pointer_cast<ui::DebugWindow>(debugWindow)) {
            // this essentially passes through all the callbacks to the main menu scene
            typedDebugWindow->ConnectToServerRequested = ConnectToServerRequested;
            typedDebugWindow->DisconnectFromServerRequested = DisconnectFromServerRequested;
            typedDebugWindow->LoginRequested = LoginRequested;
            typedDebugWindow->LogoutRequested = LogoutRequested;
        }

        inUI->AddComponent(debugWindow);
        gameLayer = std::make_shared<GameLayer>(GetWorld());
        gameLayer->SetInputSubsystem(input);

        Layers.push_back(gameLayer);
        Scene::Init(inInput, inUI);
    }

    void BaseScene::Tick(float DeltaTime) {
        // This will tick the physics world at a fixed rate


        Scene::Tick(DeltaTime);
    }


} // OZZ