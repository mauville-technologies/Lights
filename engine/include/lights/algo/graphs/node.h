//
// Created by ozzadar on 2025-07-26.
//

#pragma once
#include <memory>
#include <optional>
#include <vector>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace OZZ::lights::algo {
    /**
     * A generic node class for graph algorithms.
     * For now:
     * Nodes can have one outgoing edge to another node. (Outdegree <= 1)
     * Nodes can have multiple incoming edges from other nodes -- with a maximum of 255 (Indegree >= 0 && Indegree <= 255)
     * Essentially, this is a directed graph
     * @tparam DataType The data the node will hold
     */
    template <typename DataType>
    struct Node {
        std::unique_ptr<DataType> Data{};
        std::shared_ptr<Node> NextNode{nullptr};
        std::vector<std::shared_ptr<Node>> PreviousNodes{nullptr};

        /**
         * Points this node to OtherNode
         * @param ThisNode the node which will do the pointing
         * @param OtherNode The node to which will be pointed
         */
        static void Connect(std::shared_ptr<Node> ThisNode, std::shared_ptr<Node> OtherNode) {
            if (!ThisNode || !OtherNode) {
                spdlog::warn("Node::Connect Invalid nodes");
                return;
            }

            if (ThisNode->NextNode) {
                Disconnect(ThisNode);
            }

            ThisNode->NextNode = OtherNode;
            OtherNode->PreviousNodes.push_back(ThisNode);
        }

        /**
         * Disconnects the node from its next node
         * @param ThisNode The node which will disconnect
         */
        static void Disconnect(std::shared_ptr<Node> ThisNode) {
            if (!ThisNode) {
                spdlog::warn("Node::Disconnect Invalid node");
                return;
            }
            if (ThisNode->NextNode != nullptr) {
                std::ranges::remove_if(ThisNode->NextNode->PreviousNodes,
                                       [ThisNode](const std::shared_ptr<Node>& NodePtr) {
                                           return NodePtr.get() == ThisNode.get();
                                       });
                ThisNode->NextNode = nullptr;
            }
        }
    };

    template <typename DataType>
    std::vector<Node<DataType>*> Flatten(std::shared_ptr<Node<DataType>> RootNode) {
        std::vector<Node<DataType>*> FlattenedNodes;

        auto hasBeenVisited = [&FlattenedNodes](std::shared_ptr<Node<DataType>> NodePtr) {
            return std::ranges::find(FlattenedNodes, NodePtr.get()) != FlattenedNodes.end();
        };

        std::function<void(std::shared_ptr<Node<DataType>>)> VisitNode = [&](std::shared_ptr<Node<DataType>> NodePtr) {
            if (!NodePtr) return;
            // we assume if we've made it this far, the node hasn't been visited
            FlattenedNodes.push_back(NodePtr.get());

            // then we visit any previous node that hasn't been visited
            // and any next node that hasn't been visited
            for (const auto& PreviousNode : NodePtr->PreviousNodes) {
                if (!hasBeenVisited(PreviousNode)) {
                    VisitNode(PreviousNode);
                }
            }

            if (NodePtr->NextNode && !hasBeenVisited(NodePtr->NextNode)) {
                VisitNode(NodePtr->NextNode);
            }
        };

        VisitNode(RootNode);

        return FlattenedNodes;
    }

    template <typename DataType>
    std::optional<std::vector<DataType*>> Kahns(std::shared_ptr<Node<DataType>> RootNode) {
        auto FlattenedNodes = Flatten(RootNode);
        auto InDegreeMap = std::vector<size_t>(FlattenedNodes.size(), 0);

        // calculate the in-degree of each node
        for (auto i = 0; i < FlattenedNodes.size(); ++i) {
            const auto& NodePtr = FlattenedNodes[i];
            if (!NodePtr) continue;
            // for each previous node, increment the in-degree
            for (const auto& PreviousNode : NodePtr->PreviousNodes) {
                if (PreviousNode) {
                    InDegreeMap[i] += 1;
                }
            }
        }

        // print the in-degree map for debugging
        spdlog::debug("In-Degree Map:");
        for (size_t i = 0; i < InDegreeMap.size(); ++i) {
            spdlog::info("Node {}: {}", i, InDegreeMap[i]);
        }

        std::vector<DataType*> Result;
        auto processNode = [&FlattenedNodes, &InDegreeMap](size_t Index, std::vector<DataType*>& Result) -> size_t {
            size_t processCount = 0;
            for (auto i = 0; i < FlattenedNodes.size(); i++) {
                if (InDegreeMap[i] == 0) {
                    Result.push_back(FlattenedNodes[i]->Data.get());

                    // process the node
                    InDegreeMap[i] = std::numeric_limits<size_t>::max(); // mark as processed
                    processCount++;

                    if (const auto* NodePtr = FlattenedNodes[i]; NodePtr->NextNode) {
                        auto NextIndex = std::ranges::find_if(FlattenedNodes,
                                                              [&NodePtr](const Node<DataType>* Node) {
                                                                  return Node == NodePtr->NextNode.get();
                                                              });
                        if (NextIndex != FlattenedNodes.end()) {
                            spdlog::info("Processing node {} with next node {}", i,
                                         std::distance(FlattenedNodes.begin(), NextIndex));
                            InDegreeMap[std::distance(FlattenedNodes.begin(), NextIndex)] -= 1;
                        }
                    }
                }
            }
            return processCount;
        };

        auto processCount = 0;
        while (processCount < FlattenedNodes.size()) {
            auto processed = processNode(processCount, Result);
            processCount += processed;

            // if processCount is too big, we have a cycle
            if (processCount > FlattenedNodes.size()) {
                spdlog::warn("Kahn's algorithm detected a cycle in the graph.");
                return std::nullopt; // cycle detected
            }
        }

        return Result;
    }
}
