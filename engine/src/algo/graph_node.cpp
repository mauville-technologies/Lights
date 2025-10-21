//
// Created by ozzadar on 2025-10-20.
//

#include "lights/algo/graph_node.h"

#include "spdlog/spdlog.h"

#include <algorithm>
#include <functional>
#include <ranges>

void GraphNode::Connect(GraphNode *from, GraphNode *to) {
    auto &fromOutputs = from->outputs;
    auto &toInputs = to->inputs;
    const auto outExists = std::ranges::find_if(fromOutputs, [to](const GraphNode *node) {
        return node == to;
    });

    if (outExists == fromOutputs.end()) {
        // out connection doesn't exist, add it
        fromOutputs.emplace_back(to);
    }

    auto inExists = std::ranges::find_if(toInputs, [from](const GraphNode *node) {
        return node == from;
    });

    if (inExists == toInputs.end()) {
        // in connection doesn't exist, add it
        toInputs.emplace_back(from);
    }
}

void GraphNode::Disconnect(GraphNode *from, GraphNode *to) {
    std::erase_if(from->outputs, [to](const GraphNode *node) {
        return to == node;
    });

    std::erase_if(to->inputs, [from](const GraphNode *node) {
        return from == node;
    });
}

std::optional<std::vector<GraphNode *> > GraphNode::TopologicalSort(GraphNode *root) {
    const auto flattenedGraph = Flatten(root);
    auto inDegrees = flattenedGraph | std::views::transform([](GraphNode *node) {
                         return node->GetInDegree();
                     }) |
                     std::ranges::to<std::vector>();

    std::vector<GraphNode *> result{};

    auto decreaseInDegreesOnNode = [&inDegrees, &flattenedGraph](GraphNode *node) {
        // find the node in the graph
        for (const auto &[i, fNode]: std::views::zip(std::views::iota(0), flattenedGraph)) {
            if (node == fNode) {
                inDegrees[i] -= 1;
                return;
            }
        }
    };

    auto processNodesIteration = [&flattenedGraph, &inDegrees, &result, &decreaseInDegreesOnNode]() {
        auto nodesProcessed = 0u;

        for (auto i = 0; i < flattenedGraph.size(); i++) {
            if (inDegrees[i] == 0) {
                result.push_back(flattenedGraph[i]);

                inDegrees[i] = std::numeric_limits<size_t>::max();
                nodesProcessed++;

                // Loop through all outputs and decrease their inDegrees by 1
                for (auto *out: flattenedGraph[i]->outputs) {
                    decreaseInDegreesOnNode(out);
                }
            }
        }
        return nodesProcessed;
    };

    auto processedNodeCount = 0u;
    while (processedNodeCount < flattenedGraph.size()) {
        const auto processedNodes = processNodesIteration();

        if (processedNodes == 0) {
            spdlog::error("No processable nodes except there are nodes left to be processed");
            return std::nullopt;
        }

        processedNodeCount += processedNodes;
        if (processedNodeCount > flattenedGraph.size()) {
            spdlog::error("Detected Cycle inGraph");
            return std::nullopt;
        }
    }

    return result;
}

std::vector<GraphNode *> GraphNode::Flatten(GraphNode *root) {
    std::vector<GraphNode *> flattenedNodes{};

    auto hasBeenVisited = [&flattenedNodes](GraphNode *node) {
        return std::ranges::find(flattenedNodes, node) != flattenedNodes.end();
    };

    std::function<void(GraphNode *)> visitNode = [&](GraphNode *node) {
        if (!node)
            return;

        // we assume if we've made it this far, the node hasn't been visited
        flattenedNodes.push_back(node);

        // then we visit any previous node that hasn't been visited
        // and any next node that hasn't been visited
        for (const auto &previousNode: node->inputs) {
            if (previousNode && !hasBeenVisited(previousNode)) {
                visitNode(previousNode);
            }
        }

        for (const auto &nextNode: node->outputs) {
            if (nextNode && !hasBeenVisited(nextNode)) {
                visitNode(nextNode);
            }
        }
    };

    visitNode(root);
    return flattenedNodes;
}

bool GraphNode::AreConnected(GraphNode *from, GraphNode *to) {
    // flatten the graph, and check that both nodes are in it
    auto flattenedGraph = Flatten(from);
    return std::ranges::find(flattenedGraph, to) != flattenedGraph.end();
}

void GraphNode::ClearConnections(GraphNode *node) {
    const auto inputsCopy = node->inputs;
    const auto outputsCopy = node->outputs;

    for (auto *input: inputsCopy) {
        Disconnect(input, node);
    }
    for (auto *output: outputsCopy) {
        Disconnect(node, output);
    }
}
