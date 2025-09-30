//
// Created by ozzadar on 2025-08-12.
//

#pragma once

#include "lights/input/input_subsystem.h"

#include <clay/clay.h>
#include <lights/scene/scene_layer.h>
#include <unordered_map>

namespace OZZ {
    struct FontSet;
    class FontLoader;
} // namespace OZZ

namespace OZZ {
    class GameWorld;
}

class ClayUILayer : public OZZ::scene::SceneLayer {

private:
    struct ScissorDef {
        bool bHasScissor{false};
        Clay_BoundingBox ScissorBox{0, 0, 0, 0};
    };

public:
    explicit ClayUILayer(OZZ::GameWorld* inWorld, const std::shared_ptr<OZZ::InputSubsystem>& inInput);
    ~ClayUILayer() override;
    void Init() override;
    void PhysicsTick(float DeltaTime) override;
    void Tick(float DeltaTime) override;
    void RenderTargetResized(glm::ivec2 size) override;
    std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;

    void RegisterFont(const uint16_t& fontId, const std::filesystem::path& fontPath);
    void UnregisterFont(const uint16_t& fontId);

    void SetRenderFunction(std::function<void()> func) { componentRenderFunction = std::move(func); }

    void SetDebugPanelOpened(bool bOpened);
    [[nodiscard]] bool IsDebugPanelOpened() const;

private:
    void reinitializeClay();
    void buildShaders();
    void shutdownClay();

    void refreshSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor);
    void buildSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor);
    static bool isRenderCommandChanged(const Clay_RenderCommand& command, const Clay_RenderCommand& otherCommand);

    void generateSquareMesh(std::vector<OZZ::Vertex>& outVertices,
                            std::vector<uint32_t>& outIndices,
                            glm::vec2 scale = {1.f, 1.f});
    void generateTextMesh(const std::string& text,
                          OZZ::FontSet* fontSet,
                          std::vector<OZZ::Vertex>& outVertices,
                          std::vector<uint32_t>& outIndices,
                          const glm::vec2& baseline = {0, 0});

private:
    std::function<void()> componentRenderFunction;
    std::shared_ptr<OZZ::InputSubsystem> inputSubsystem;

    glm::ivec2 screenSize{0, 0};

    // Clay things
    bool bClayInitialized{false};
    uint32_t clayTotalMemorySize{0};
    Clay_Arena clayArena{};
    Clay_RenderCommandArray clayRenderCommandArray{};

    std::unique_ptr<OZZ::FontLoader> fontLoader{nullptr};

    // Scene objects materials
    std::shared_ptr<OZZ::Shader> uiShader{};
    std::shared_ptr<OZZ::Shader> textShader{};
    std::unordered_map<uint32_t, OZZ::scene::SceneObject> uiSceneObjects{};
    std::unordered_map<uint32_t, Clay_RenderCommand> currentRenderCommand{};

    std::unordered_map<std::filesystem::path, std::shared_ptr<OZZ::Texture>> uiImages{};
    std::unordered_map<uint16_t, std::filesystem::path> fontRegistry{};
};
