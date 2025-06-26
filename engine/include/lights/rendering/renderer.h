//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/scene/scene.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();

        void RenderScene(scene::Scene* scene);
    };
}