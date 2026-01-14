//
// Created by ozzadar on 2024-12-18.
//

#include "lights/rendering/renderer.h"
#include "glad/glad.h"
#include "lights/rendering/renderables/renderable_viewport.h"

namespace OZZ {
    Renderer::Renderer() {
        viewport = std::make_unique<RenderableViewport>();
    }

    void Renderer::Init() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);

        viewport->Init();
    }

    void Renderer::ExecuteSceneGraph(Renderable* sceneGraph) {
        // Clear current connections
        GraphNode::ClearConnections(viewport.get());

        // TODO: @paulm validate the sceneGraph

        // Connect the sceneGraph to the viewport
        GraphNode::Connect(sceneGraph, viewport.get());

        // Get the dependency list
        auto orderedGraph = GraphNode::TopologicalSort(viewport.get());
        if (!orderedGraph) {
            spdlog::critical("Failed to topological sort.");
            return;
        }

        // Reset frame state for all nodes
        for (const auto node : orderedGraph.value()) {
            auto* renderNode = static_cast<OZZ::Renderable*>(node);
            renderNode->ResetFrameState();
        }

        for (const auto node : orderedGraph.value()) {
            auto* renderNode = static_cast<OZZ::Renderable*>(node);
            if (!renderNode->Render()) {
                spdlog::error("Failed to render node {}", renderNode->GetRenderableName());
            }
        }
    }
} // namespace OZZ
