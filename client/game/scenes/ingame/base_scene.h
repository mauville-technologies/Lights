//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/game/world.h"
#include "lights/scene/scene.h"
#include "lights/ui/user_interface.h"

#include "game/callback_functions.h"
#include "layers/game_layer.h"

namespace OZZ::game::scene {


    class BaseScene : public Scene {
    public:
        explicit BaseScene(GetApplicationStateFunction inAppStateFunction);
        ~BaseScene() override;

        void Init(std::shared_ptr<InputSubsystem> inInput, std::shared_ptr<UserInterface> inUI) override;
        void Tick(float DeltaTime) override;


        ConnectToServerRequestedFunction ConnectToServerRequested;
        DisconnectFromServerRequestedFunction DisconnectFromServerRequested;
        LoginRequestedFunction LoginRequested;
        LogoutRequestedFunction LogoutRequested;

    private:
        GetApplicationStateFunction appStateFunction;

        std::shared_ptr<GameLayer> gameLayer;
        std::shared_ptr<UIComponent> debugWindow;
    };

} // OZZ