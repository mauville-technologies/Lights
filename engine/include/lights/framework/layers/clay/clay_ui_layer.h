//
// Created by ozzadar on 2025-08-12.
//

#pragma once

#include "lights/core/rendering/renderable.h"

#include <clay/clay.h>
#include <lights/framework/scene/scene_layer.h>
#include <unordered_map>

#include "lights/core/text/font_loader.h"

namespace OZZ {
    struct FontSet;
} // namespace OZZ

class ClayUILayer : public OZZ::scene::SceneLayer, public OZZ::Renderable {
private:
    struct CameraSettings {
        glm::mat4 View;
        glm::mat4 Proj;
    };

    struct UIComponentSettings {
        glm::vec4 BackgroundColor;
        glm::vec4 BorderColor;
        glm::vec4 BorderWidth;   // left, right, top, bottom
        glm::vec4 BorderRadiusX; // top-left, top-right, bottom-right, bottom-left
        glm::vec4 BorderRadiusY; // top-left, top-right, bottom-right, bottom-left
    };

    // Shared preamble: UBOs, texture, sampler, push constant, vertex entry point.
    // Included in both UISlangShader and FontUISlangShader.
    static constexpr const char* kClayShaderPreamble = R"(
struct VertexOutput {
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

struct CameraSettings {
    float4x4 view;
    float4x4 proj;
};

struct UIComponentSettings {
    float4 backgroundColor;
    float4 borderColor;
    float4 borderWidth;    // left, right, top, bottom
    float4 borderRadiusX;  // top-left, top-right, bottom-right, bottom-left
    float4 borderRadiusY;  // top-left, top-right, bottom-right, bottom-left
};

[vk::binding(0, 0)] ConstantBuffer<CameraSettings> camera;
[vk::binding(1, 0)] ConstantBuffer<UIComponentSettings> uiSettings;
[vk::binding(2, 0)] Texture2D<float4> image;
[vk::binding(3, 0)] SamplerState imageSmp;

struct PushConstants { float4x4 model; };
[[vk::push_constant]]
ConstantBuffer<PushConstants> pc;

[shader("vertex")]
VertexOutput vertexMain(
    [vk::location(0)] float3 aPos,
    [vk::location(1)] float4 aColor,
    [vk::location(2)] float3 aNormal,
    [vk::location(3)] float2 aTexCoord
) {
    VertexOutput output;
    output.position = mul(mul(mul(camera.proj, camera.view), pc.model), float4(aPos, 1.0));
    output.texCoord = aTexCoord;
    return output;
}
)";

    const std::string UISlangShader = std::string(kClayShaderPreamble) + R"(
float ellipseMask(float2 uv, float2 center, float rx, float ry) {
    float2 norm = (uv - center) / float2(rx, ry);
    return smoothstep(0.9, 1.1, dot(norm, norm));
}

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target {
    float4 backgroundColor = uiSettings.backgroundColor;
    float4 borderColor     = uiSettings.borderColor;
    float4 borderWidth     = uiSettings.borderWidth;
    float4 borderRadiusX   = uiSettings.borderRadiusX;
    float4 borderRadiusY   = uiSettings.borderRadiusY;

    float2 texCoord = input.texCoord;
    float mask = 1.0;

    if (texCoord.x < borderRadiusX.x && texCoord.y < borderRadiusY.x) {
        mask = 1.0 - ellipseMask(texCoord, float2(borderRadiusX.x, borderRadiusY.x), borderRadiusX.x, borderRadiusY.x);
    } else if (texCoord.x > 1.0 - borderRadiusX.y && texCoord.y < borderRadiusY.y) {
        mask = 1.0 - ellipseMask(texCoord, float2(1.0 - borderRadiusX.y, borderRadiusY.y), borderRadiusX.y, borderRadiusY.y);
    } else if (texCoord.x > 1.0 - borderRadiusX.z && texCoord.y > 1.0 - borderRadiusY.z) {
        mask = 1.0 - ellipseMask(texCoord, float2(1.0 - borderRadiusX.z, 1.0 - borderRadiusY.z), borderRadiusX.z, borderRadiusY.z);
    } else if (texCoord.x < borderRadiusX.w && texCoord.y > 1.0 - borderRadiusY.w) {
        mask = 1.0 - ellipseMask(texCoord, float2(borderRadiusX.w, 1.0 - borderRadiusY.w), borderRadiusX.w, borderRadiusY.w);
    }

    float maskLeft   = 1.0 - step(borderWidth.x, texCoord.x);
    float maskRight  = 1.0 - step(borderWidth.y, 1.0 - texCoord.x);
    float maskTop    = 1.0 - step(borderWidth.z, texCoord.y);
    float maskBottom = 1.0 - step(borderWidth.w, 1.0 - texCoord.y);

    float borderMask      = max(max(maskLeft, maskRight), max(maskTop, maskBottom));
    float finalBorderMask = mask * borderMask;

    float4 backgroundColorFinal = lerp(backgroundColor, float4(0.0, 0.0, 0.0, 0.0), 1.0 - mask);
    float4 imageColor           = lerp(image.Sample(imageSmp, texCoord), float4(0.0, 0.0, 0.0, 0.0), 1.0 - mask);
    return lerp(lerp(backgroundColorFinal, borderColor, finalBorderMask), imageColor, imageColor.a);
}
)";

    const std::string FontUISlangShader = std::string(kClayShaderPreamble) + R"(
[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target {
    float sampled = image.Sample(imageSmp, input.texCoord).r;
    return float4(1.0, 1.0, 1.0, sampled) * uiSettings.borderColor;
}
)";

    struct ScissorDef {
        bool bHasScissor{false};
        Clay_BoundingBox ScissorBox{0, 0, 0, 0};
    };

public:
    ClayUILayer();

    ~ClayUILayer() override;

    void Init(OZZ::rendering::RHIDevice* inDevice) override;

    void PhysicsTick(float DeltaTime) override;

    void Tick(float DeltaTime) override;

    void RenderTargetResized(glm::ivec2 size) override;

    void RegisterFont(const uint16_t& fontId, const std::filesystem::path& fontPath);

    void UnregisterFont(const uint16_t& fontId);

    void SetTickDefinitionFunction(std::function<void()> func) { tickDefinitionFunction = std::move(func); }

    void SetDebugPanelOpened(bool bOpened);

    [[nodiscard]] bool IsDebugPanelOpened() const;

private:
    void reinitializeClay();

    void buildShaders();

    void shutdownClay();

    void refreshSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor);

    void buildSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor);

    std::vector<OZZ::scene::SceneObject> getSceneObjects();

    static bool isRenderCommandChanged(const Clay_RenderCommand& command, const Clay_RenderCommand& otherCommand);

    void generateSquareMesh(std::vector<OZZ::Vertex>& outVertices,
                            std::vector<uint32_t>& outIndices,
                            glm::vec2 scale = {1.f, 1.f});

    void generateTextMesh(const std::string& text,
                          OZZ::FontSet* fontSet,
                          std::vector<OZZ::Vertex>& outVertices,
                          std::vector<uint32_t>& outIndices);

public:
    std::string GetRenderableName() override { return "ClayUILayer"; }

protected:
    bool render(OZZ::rendering::RHIFrameContext& frameContext) override;

private:
    OZZ::rendering::RHIBufferHandle cameraBuffer{OZZ::rendering::RHIBufferHandle::Null()};
    std::function<void()> tickDefinitionFunction;
    glm::ivec2 screenSize{0, 0};

    // Clay things
    bool bClayInitialized{false};
    uint32_t clayTotalMemorySize{0};
    Clay_Arena clayArena{};
    Clay_RenderCommandArray clayRenderCommandArray{};

    std::unique_ptr<OZZ::FontLoader> fontLoader{nullptr};
    std::unique_ptr<OZZ::RenderTarget> renderTarget{nullptr};

    // Scene objects materials
    std::shared_ptr<OZZ::Shader> uiShader{};
    std::shared_ptr<OZZ::Shader> textShader{};
    std::unordered_map<uint32_t, OZZ::scene::SceneObject> uiSceneObjects{};
    std::unordered_map<uint32_t, OZZ::rendering::RHIBufferHandle> uiComponentSettings{};

    std::unordered_map<uint32_t, Clay_RenderCommand> currentRenderCommand{};

    // Image wrappers consumed once per Tick; render() may be called multiple times per frame safely.
    std::unordered_map<uint32_t, std::shared_ptr<OZZ::Texture>> frameImageTextures{};

    std::unordered_map<std::filesystem::path, std::shared_ptr<OZZ::Texture>> uiImages{};
    std::unordered_map<uint16_t, std::filesystem::path> fontRegistry{};

    // Reused across buildSceneObject calls to avoid per-frame heap allocations.
    std::vector<OZZ::Vertex> meshVerticesWorkspace{};
    std::vector<uint32_t> meshIndicesWorkspace{};
};
