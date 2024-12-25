//
// Created by ozzadar on 2024-12-24.
//

#pragma once
#include <string>


enum class ConnectionState : uint8_t {
    Disconnected,
    Connecting,
    Connected,
    Disconnecting
};

enum class LoginState : uint8_t {
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
    ConnectionState ConnectionState{ConnectionState::Disconnected};
    LoginState LoginState{LoginState::NotLoggedIn};

    // Player State
    PlayerState PlayerState;
};