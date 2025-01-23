//
// Created by ozzadar on 2024-12-22.
//

#pragma once

#include "lights/ui/user_interface.h"
#include "game/application_state.h"
#include "game/callback_functions.h"
#include <unordered_map>
#include <string>

namespace OZZ::game::ui {

    class DebugWindow : public UIComponent {
    public:
        explicit DebugWindow(GetApplicationStateFunction  inAppStateFunction);

        ConnectToServerRequestedFunction ConnectToServerRequested;
        DisconnectFromServerRequestedFunction DisconnectFromServerRequested;
        LoginRequestedFunction LoginRequested;
        LogoutRequestedFunction LogoutRequested;

    private:
        void registerInDockspace() override;
        void render() override;
    private:
        GetApplicationStateFunction appStateFunction;
        std::unordered_map<std::string, std::shared_ptr<UIComponent>> components;
    };

} // OZZ