//
// Created by paulm on 2025-10-21.
//

#pragma once

#include "lights/core/rendering/renderable.h"
#include "lights/framework/scene/scene_object.h"

namespace OZZ {

    class RenderableViewport : public Renderable {
    public:
        RenderableViewport();

        void Init(rendering::RHIDevice* inDevice);

        std::string GetRenderableName() override { return "RenderableViewport"; }

        constexpr std::vector<std::string> GetRequiredInputs() override { return {"ViewportTexture"}; }

        void Resize(glm::uvec2 size);

    protected:
        bool render(rendering::RHIFrameContext& frameContext) override;

    private:
        rendering::RHIDevice* device;
        scene::SceneObject sceneObject;
        std::unique_ptr<RenderTarget> renderTarget{nullptr};
    };
} // namespace OZZ
