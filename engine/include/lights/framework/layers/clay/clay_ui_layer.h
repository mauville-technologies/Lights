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
