//
// Created by paulm on 2025-10-21.
//

#pragma once
#include <lights/core/algo/graph_node.h>
#include <lights/core/rendering/render_target.h>

#include <spdlog/spdlog.h>
#include <string>

namespace OZZ {
    class Renderable : public GraphNode {
    public:
        virtual ~Renderable() = default;

        virtual std::string GetRenderableName() = 0;

        virtual std::vector<std::string> GetRequiredInputs() { return {}; }

        std::optional<RenderTarget*> GetRender(const std::string& name);

        bool Render();

        void ResetFrameState();

    protected:
        bool HasAllRequiredInputs();

        virtual bool render() = 0;

        virtual void newFrame() {}

    protected:
        bool bRenderedThisFrame{false};
        std::unordered_map<std::string, RenderTarget*> renders{};
    };
} // namespace OZZ
