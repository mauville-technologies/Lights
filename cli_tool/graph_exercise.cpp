//
// Created by ozzadar on 2025-10-19.
//

#include "graph_exercise.h"

Renderer::Renderer() {
    viewport = std::make_unique<Viewport>();
}

void Renderer::ExecuteTheGraph(OZZ::Renderable *sceneGraph) {
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
    for (const auto node: orderedGraph.value()) {
        auto *renderNode = static_cast<OZZ::Renderable *>(node);
        renderNode->ResetFrameState();
    }

    for (const auto node: orderedGraph.value()) {
        auto *renderNode = static_cast<OZZ::Renderable *>(node);
        if (!renderNode->Render()) {
            spdlog::error("Failed to render node {}", renderNode->GetName());
        }
    }
}

Game::Game() {
    renderer = std::make_unique<Renderer>();
    scene = std::make_unique<Scene>();
}

void Game::Run() {
    while (true) {
        spdlog::info("-----------------------------");
        auto sceneGraphNode = scene->GetSceneGraph();
        renderer->ExecuteTheGraph(sceneGraphNode);
        spdlog::info("-----------------------------");
    }
}

Scene::Scene() {
    // Build the scene graph
    texture = std::make_unique<PGTexture>();
    layer = std::make_unique<UILayer>();
    game = std::make_unique<GameLayer>();
    aggregator = std::make_unique<Aggregator>();

    OZZ::Renderable::Connect(texture.get(), game.get());
    OZZ::Renderable::Connect(texture.get(), layer.get());
    OZZ::Renderable::Connect(game.get(), aggregator.get());
    OZZ::Renderable::Connect(layer.get(), aggregator.get());
}

OZZ::Renderable *Scene::GetSceneGraph() const {
    return aggregator.get();
}
