//
// Created by ozzadar on 2025-10-19.
//

#pragma once
#include <ranges>

#include "node.h"
#include "spdlog/spdlog.h"

struct RenderTarget {
    std::string data;
    bool bRendered{false};

    void NewFrame() {
        bRendered = false;
    }
};

class Renderable : public GraphNode {
public:
    virtual ~Renderable() = default;

    virtual std::string GetName() = 0;

    virtual std::vector<std::string> GetRequiredInputs() { return {}; }

    std::optional<RenderTarget *> GetRender(const std::string &name) {
        if (bRenderedThisFrame && renders.contains(name)) {
            if (&renders[name].bRendered) {
                // found, and rendered
                return &renders[name];
            }
            // found, but not rendered
            return std::nullopt;
        }
        // not found
        return std::nullopt;
    };

    bool Render() {
        if (!HasAllRequiredInputs()) {
            spdlog::warn("Cannot render node {} due to missing inputs", GetName());
            return false;
        }

        if (render()) {
            bRenderedThisFrame = true;
            return true;
        }
        return false;
    }

    void ResetFrameState() {
        bRenderedThisFrame = false;
        for (auto &renderTarget: renders | std::views::values) {
            renderTarget.NewFrame();
        }
        newFrame();
    }

protected:
    bool HasAllRequiredInputs() {
        for (const auto &requiredInput: GetRequiredInputs()) {
            bool found = false;
            for (const auto &inputNode: inputs) {
                auto *renderableNode = static_cast<Renderable *>(inputNode);
                if (renderableNode->GetRender(requiredInput)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                spdlog::warn("Missing required input {} for node {}", requiredInput, GetName());
                return false;
            }
        }
        return true;
    }

    virtual bool render() = 0;

    virtual void newFrame() {
    }

protected:
    bool bRenderedThisFrame{false};
    std::unordered_map<std::string, RenderTarget> renders{};
};

class PGTexture : public Renderable {
public:
    PGTexture() {
        auto newTarget = RenderTarget();
        newTarget.data = "TextureData";
        renders.insert(std::make_pair("PGTexture", newTarget));
    }

    std::string GetName() override { return "PGTexture"; }

protected:
    bool render() override {
        spdlog::info("Rendering texture");
        renders["PGTexture"].bRendered = true;
        return true;
    }
};

class UILayer : public Renderable {
public:
    std::string GetName() override { return "UILayer"; }

    std::vector<std::string> GetRequiredInputs() override { return {"PGTexture"}; }

protected:
    bool render() override {
        spdlog::info("Rendering UI");
        // look for texture input
        for (const auto &inputNode: inputs) {
            auto *renderableNode = static_cast<Renderable *>(inputNode);
            auto render = renderableNode->GetRender("PGTexture");
            if (render) {
                spdlog::info("Found render texture");
                return true;
            }
        }

        return false;
    }
};

class GameLayer : public Renderable {
public:
    std::string GetName() override { return "GameLayer"; }

protected:
    bool render() override {
        spdlog::info("Rendering game");
        // look for texture input
        for (const auto &inputNode: inputs) {
            auto *renderableNode = static_cast<Renderable *>(inputNode);
            auto render = renderableNode->GetRender("PGTexture");
            if (render) {
                spdlog::info("Found render texture");
                return true;
            }
        }

        return false;
    }
};

class Aggregator : public Renderable {
public:
    std::string GetName() override { return "Aggregator"; }

protected:
    bool render() override {
        spdlog::info("Rendering aggregator");
        return true;
    }
};

class Viewport : public Renderable {
public:
    std::string GetName() override { return "Viewport"; }

protected:
    bool render() override {
        spdlog::info("Rendering viewport");
        return true;
    }
};

class Scene : public GraphNode {
public:
    Scene();

    [[nodiscard]] Renderable *GetSceneGraph() const;

private:
    std::unique_ptr<PGTexture> texture;
    std::unique_ptr<UILayer> layer;
    std::unique_ptr<GameLayer> game;

    std::unique_ptr<Aggregator> aggregator;
};

class Renderer {
public:
    Renderer();

    void ExecuteTheGraph(Renderable *sceneGraph);

private:
    std::unique_ptr<Renderable> viewport;
};

class Game {
public:
    Game();

    void Run();

private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> scene;
};
