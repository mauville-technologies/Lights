//
// Created by ozzadar on 2024-12-24.
//

#pragma once
#include <string>
#include <functional>
#include "game/application_state.h"

namespace OZZ {
    // TODO: Consider other patterns such as an event bus rather than this gaggle of callbacks
    using ConnectToServerRequestedFunction = std::function<void()>;
    using DisconnectFromServerRequestedFunction = std::function<void()>;
    using LoginRequestedFunction = std::function<void(const std::string &, const std::string &)>;
    using LogoutRequestedFunction = std::function<void()>;

    using GetApplicationStateFunction = std::function<const struct ApplicationState()>;
}
