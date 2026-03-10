//
// Created by paulm on 2025-10-21.
//

#pragma once

#include "lights/core/rendering/renderable.h"
#include "lights/framework/scene/scene_object.h"

namespace OZZ {

    class RenderableViewport : public Renderable {
    public:
        const std::string VertexShader = R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

layout (location = 0) out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    texCoord = aTexCoord;
}
)";

        const std::string FragmentShader = R"(
#version 450 core

layout (location = 0) in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D inTexture;

void main()
{
    FragColor = texture(inTexture, texCoord);
}
)";
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
