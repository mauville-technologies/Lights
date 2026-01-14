//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/scene/scene.h"
#include "renderable.h"
#include "renderables/renderable_viewport.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();

        void Init();

        void ExecuteSceneGraph(Renderable* sceneGraph);

    private:
        std::unique_ptr<RenderableViewport> viewport{nullptr};
    };
} // namespace OZZ
