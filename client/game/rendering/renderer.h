//
// Created by ozzadar on 2024-12-18.
//

#pragma once
#include <memory>
#include "game/scene/scene.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer() = default;

        void RenderScene(Scene* scene);
    };
}