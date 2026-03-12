//
// Created by ozzadar on 2025-08-12.
//

#pragma once

#include "lights/core/rendering/renderable.h"
#include "lights/framework/input/input_subsystem.h"

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

    const std::string VertexShader = R"(
#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

layout (location = 1) out vec2 texCoord;

layout(set = 0, binding = 0) uniform CameraSettings {
    mat4 view;
    mat4 proj;
} camera;

layout(push_constant) uniform PushConstants {
    mat4 model;
} pc;

void main()
{
    gl_Position = camera.proj * camera.view * pc.model * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}

)";

    const std::string FragmentShader = R"(
#version 450 core
layout (location = 0) out vec4 FragColor;

layout (location = 1) in vec2 texCoord;

layout(set = 0, binding = 1) uniform UIComponentSettings {
    vec4 backgroundColor;
    vec4 borderColor;
    vec4 borderWidth;
    vec4 borderRadiusX;
    vec4 borderRadiusY;
} uiSettings;

layout(set = 0, binding = 2) uniform sampler2D image;

float ellipseMask(vec2 uv, vec2 center, float rx, float ry) {
    vec2 norm = (uv - center) / vec2(rx, ry);
    return smoothstep(0.9, 1.1, dot(norm, norm));
}

void main()
{
    vec4 backgroundColor = uiSettings.backgroundColor;
    vec4 borderColor = uiSettings.borderColor;
    vec4 borderWidth = uiSettings.borderWidth;
    vec4 borderRadiusX = uiSettings.borderRadiusX;
    vec4 borderRadiusY = uiSettings.borderRadiusY;

    float mask = 1.0;

    if (texCoord.x < borderRadiusX.x && texCoord.y < borderRadiusY.x) {
        mask = 1.0 - ellipseMask(texCoord, vec2(borderRadiusX.x, borderRadiusY.x), borderRadiusX.x, borderRadiusY.x);
    } else if (texCoord.x > 1.0 - borderRadiusX.y && texCoord.y < borderRadiusY.y) {
        mask = 1.0 - ellipseMask(texCoord, vec2(1.0 - borderRadiusX.y, borderRadiusY.y), borderRadiusX.y, borderRadiusY.y);
    } else if (texCoord.x > 1.0 - borderRadiusX.z && texCoord.y > 1.0 - borderRadiusY.z) {
        mask = 1.0 - ellipseMask(texCoord, vec2(1.0 - borderRadiusX.z, 1.0 - borderRadiusY.z), borderRadiusX.z, borderRadiusY.z);
    } else if (texCoord.x < borderRadiusX.w && texCoord.y > 1.0 - borderRadiusY.w) {
        mask = 1.0 - ellipseMask(texCoord, vec2(borderRadiusX.w, 1.0 - borderRadiusY.w), borderRadiusX.w, borderRadiusY.w);
    }

    float distanceLeft = texCoord.x;
    float distanceRight = 1.0 - texCoord.x;
    float distanceTop = texCoord.y;
    float distanceBottom = 1.0 - texCoord.y;

    float maskLeft = 1.0 - step(borderWidth.x, distanceLeft);
    float maskRight = 1.0 - step(borderWidth.y, distanceRight);
    float maskTop = 1.0 - step(borderWidth.z, distanceTop);
    float maskBottom = 1.0 - step(borderWidth.w, distanceBottom);

    float maskX = max(maskLeft, maskRight);
    float maskY = max(maskTop, maskBottom);
    float borderMask = max(maskX, maskY);

    float finalBorderMask = mask * borderMask;
    float backgroundMask = mask;

    vec4 backgroundColorFinal = mix(backgroundColor, vec4(0.0), 1 - mask);
    vec4 imageColor = mix(texture(image, texCoord), vec4(0.0), 1 - mask);
    FragColor = mix(mix(backgroundColorFinal, borderColor, finalBorderMask), imageColor, imageColor.a);
}

)";

    const std::string FontFragmentShader = R"(
#version 450 core
layout (location = 0) out vec4 FragColor;

layout (location = 1) in vec2 texCoord;


layout(set = 0, binding = 1) uniform UIComponentSettings {
    vec4 backgroundColor;
    vec4 borderColor;
    vec4 borderWidth;
    vec4 borderRadiusX;
    vec4 borderRadiusY;
} uiSettings;


layout(binding = 2) uniform sampler2D image;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(image, texCoord).r);
    FragColor = vec4(uiSettings.borderColor) * sampled;
}
        )";

    struct ScissorDef {
        bool bHasScissor{false};
        Clay_BoundingBox ScissorBox{0, 0, 0, 0};
    };

public:
    explicit ClayUILayer(OZZ::InputSubsystem* inInput);

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
                          std::vector<uint32_t>& outIndices,
                          const glm::vec2& baseline = {0, 0});

public:
    std::string GetRenderableName() override { return "ClayUILayer"; }

protected:
    bool render(OZZ::rendering::RHIFrameContext& frameContext) override;

private:
    OZZ::rendering::RHIBufferHandle cameraBuffer{OZZ::rendering::RHIBufferHandle::Null()};
    std::function<void()> tickDefinitionFunction;
    OZZ::InputSubsystem* inputSubsystem;

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

    std::unordered_map<std::filesystem::path, std::shared_ptr<OZZ::Texture>> uiImages{};
    std::unordered_map<uint16_t, std::filesystem::path> fontRegistry{};
};
