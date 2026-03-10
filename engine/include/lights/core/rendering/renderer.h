//
// Created by ozzadar on 2024-12-18.
//

#pragma once

#include "lights/framework/scene/scene.h"
#include "renderable.h"
#include "renderables/renderable_viewport.h"

namespace OZZ {
    class Renderer {
    public:
        Renderer();
        ~Renderer();

        void Init(rendering::PlatformContext&& platformContext);
        void ExecuteSceneGraph(Renderable* sceneGraph);

        void ViewportResize(const glm::uvec2 newSize) const {
            if (viewport) {
                viewport->Resize(newSize);
            }
        }

        [[nodiscard]] rendering::RHIDevice* GetDevice() const { return device.get(); }

    private:
        std::unique_ptr<RenderableViewport> viewport{nullptr};
        std::unique_ptr<rendering::RHIDevice> device{nullptr};
    };
} // namespace OZZ
