//
// Created by paulm on 2025-10-21.
//

#pragma once

#include "lights/core/rendering/renderable.h"
#include "lights/framework/scene/scene_object.h"

namespace OZZ {

    class RenderableViewport : public Renderable {
    public:
        const std::string ViewportShader = R"(
struct VertexOutput {
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

[vk::combinedImageSampler] [vk::binding(1, 0)] Texture2D<float4> inTexture;
[vk::combinedImageSampler] [vk::binding(1, 0)] SamplerState      inTextureSmp;

[shader("vertex")]
VertexOutput vertexMain(
    [vk::location(0)] float3 aPos,
    [vk::location(1)] float4 aColor,
    [vk::location(2)] float3 aNormal,
    [vk::location(3)] float2 aTexCoord
) {
    VertexOutput output;
    output.position = float4(aPos, 1.0);
    output.texCoord = aTexCoord;
    return output;
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target {
    return inTexture.Sample(inTextureSmp, input.texCoord);
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
