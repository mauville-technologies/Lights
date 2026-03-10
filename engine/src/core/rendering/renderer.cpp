//
// Created by ozzadar on 2024-12-18.
//

#include "lights/core/rendering/renderer.h"
#include "lights/core/rendering/renderables/renderable_viewport.h"

namespace OZZ {
    Renderer::Renderer() {
        viewport = std::make_unique<RenderableViewport>();
    }

    Renderer::~Renderer() {
        viewport.reset();
        device.reset();
    }

    void Renderer::Init(rendering::PlatformContext&& platformContext) {
        device = rendering::CreateRHIDevice({
            .Backend = rendering::RHIBackend::Auto,
            .Context = std::move(platformContext),
        });
        viewport->Init(device.get());
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

        auto frameContext = device->BeginFrame();
        for (const auto node : orderedGraph.value()) {
            auto* renderNode = static_cast<OZZ::Renderable*>(node);
            if (!renderNode->Render(frameContext)) {
                spdlog::error("Failed to render node {}", renderNode->GetRenderableName());
            }
        }
        device->SubmitAndPresentFrame(std::move(frameContext));
    }
} // namespace OZZ
