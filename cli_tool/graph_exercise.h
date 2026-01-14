//
// Created by ozzadar on 2025-10-19.
//
/**
 * TIP: THIS IS BORKED AFTER MIGRATION -- don't use!!
 */
#pragma once
#include <ranges>

#include "lights/algo/graph_node.h"
#include "lights/rendering/renderable.h"
#include "spdlog/spdlog.h"

class PGTexture : public OZZ::Renderable {
public:
    PGTexture() {
        newTarget = std::make_unique<OZZ::RenderTarget>(OZZ::RenderTargetType::Texture);
        renders.insert(std::make_pair("PGTexture", newTarget.get()));
    }

    std::string GetRenderableName() override { return "PGTexture"; }

protected:
    bool render() override {
        spdlog::info("Rendering texture");
        return true;
    }

private:
    std::unique_ptr<OZZ::RenderTarget> newTarget;
};

class UILayer : public OZZ::Renderable {
public:
    std::string GetRenderableName() override { return "UILayer"; }

    std::vector<std::string> GetRequiredInputs() override { return {"PGTexture"}; }

protected:
    bool render() override {
        spdlog::info("Rendering UI");
        // look for texture input
        for (const auto& inputNode : inputs) {
            auto* renderableNode = static_cast<Renderable*>(inputNode);
            auto render = renderableNode->GetRender("PGTexture");
            if (render) {
                spdlog::info("Found render texture");
                return true;
            }
        }

        return false;
    }
};

class GameLayer : public OZZ::Renderable {
public:
    std::string GetRenderableName() override { return "GameLayer"; }

protected:
    bool render() override {
        spdlog::info("Rendering game");
        // look for texture input
        for (const auto& inputNode : inputs) {
            auto* renderableNode = static_cast<Renderable*>(inputNode);
            auto render = renderableNode->GetRender("PGTexture");
            if (render) {
                spdlog::info("Found render texture");
                return true;
            }
        }

        return false;
    }
};

class Aggregator : public OZZ::Renderable {
public:
    std::string GetRenderableName() override { return "Aggregator"; }

protected:
    bool render() override {
        spdlog::info("Rendering aggregator");
        return true;
    }
};

class Viewport : public OZZ::Renderable {
public:
    std::string GetRenderableName() override { return "Viewport"; }

protected:
    bool render() override {
        spdlog::info("Rendering viewport");
        return true;
    }
};

class Scene {
public:
    Scene();

    [[nodiscard]] OZZ::Renderable* GetSceneGraph() const;

private:
    std::unique_ptr<PGTexture> texture;
    std::unique_ptr<UILayer> layer;
    std::unique_ptr<GameLayer> game;

    std::unique_ptr<Aggregator> aggregator;
};

class Renderer {
public:
    Renderer();

    void ExecuteTheGraph(OZZ::Renderable* sceneGraph);

private:
    std::unique_ptr<OZZ::Renderable> viewport;
};

class Game {
public:
    Game();

    void Run();

private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> scene;
};
