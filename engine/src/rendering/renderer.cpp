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

    void Renderer::RenderScene(scene::Scene* scene) {
        const auto [ClearColor] = scene->Params;
        glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Render the scene
        // Render all scene objects
        for (auto& layer : scene->GetActiveLayers()) {
            for (auto& object : layer->GetSceneObjects()) {
                auto& [transform, objMesh, objMat] = object;
                objMat->Bind();
                objMat->GetShader()->SetMat4("view", layer->GetCamera().ViewMatrix);
                objMat->GetShader()->SetMat4("projection", layer->GetCamera().ProjectionMatrix);
                objMat->GetShader()->SetMat4("model", transform);
                objMesh->Bind();
                const auto drawMode = ToGLEnum(objMat->GetSettings().Mode);
                glDrawElements(drawMode, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            }
        }

        // TODO: Unbind the render target
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
                spdlog::error("Failed to render node {}", renderNode->GetName());
            }
        }
    }
} // namespace OZZ
