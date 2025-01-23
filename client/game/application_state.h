//
// Created by ozzadar on 2024-12-24.
//

#pragma once
#include <string>


namespace OZZ::game {
    enum class ApplicationConnectionState : uint8_t {
        Disconnected,
        Connecting,
        Connected,
        Disconnecting
    };

    enum class ApplicationLoginState : uint8_t {
        NotLoggedIn,
        LoggingIn,
        LoggedIn,
        LoggingOut
    };

    struct PlayerState {
        std::string Username;

        // TODO: Add more player state here

        inline void Clear() {
            Username.clear();
        }
    };

    struct ApplicationState {
        // Network / Connection State
        ApplicationConnectionState ConnectionState{ApplicationConnectionState::Disconnected};
        ApplicationLoginState LoginState{ApplicationLoginState::NotLoggedIn};

        // Player State
        PlayerState CurrentPlayerState;
    };
}