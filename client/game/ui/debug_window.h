//
// Created by ozzadar on 2024-12-22.
//

#pragma once

#include "lights/ui/user_interface.h"

namespace OZZ {

    class DebugWindow : public UIComponent {
    private:
        void registerInDockspace() override;
        void render() override;

    private:

    };

} // OZZ