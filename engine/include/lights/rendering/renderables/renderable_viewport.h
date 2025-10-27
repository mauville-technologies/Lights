//
// Created by paulm on 2025-10-21.
//

#pragma once

#include "lights/rendering/renderable.h"
#include "lights/scene/scene_object.h"

namespace OZZ {

    class RenderableViewport : public Renderable {
    public:
        const std::string VertexShader = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    texCoord = aTexCoord;
}
)";

        const std::string FragmentShader = R"(
#version 410 core
out vec4 FragColor;

in vec4 ourColor;
in vec2 texCoord;

uniform sampler2D inTexture;

void main()
{
    FragColor = texture(inTexture, texCoord);
}
)";
        RenderableViewport();

        void Init();

        std::string GetName() override { return "RenderableViewport"; }

        constexpr std::vector<std::string> GetRequiredInputs() override { return {"ViewportTexture"}; }

    protected:
        bool render() override;

    private:
        scene::SceneObject sceneObject;
        std::unique_ptr<RenderTarget> renderTarget{nullptr};
    };
} // namespace OZZ
