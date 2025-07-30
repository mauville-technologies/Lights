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
    struct NodeBase : std::enable_shared_from_this<NodeBase> {
        virtual ~NodeBase() = default;
        std::shared_ptr<NodeBase> NextNode{nullptr};
        std::vector<std::shared_ptr<NodeBase>> PreviousNodes{};

        /**
         * Points this node to OtherNode
         * @param ThisNode the node which will do the pointing
         * @param OtherNode The node to which will be pointed
         */
        static void Connect(const std::shared_ptr<NodeBase>& ThisNode,
                            const std::shared_ptr<NodeBase>& OtherNode) {
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
        static void Disconnect(const std::shared_ptr<NodeBase>& ThisNode) {
            if (!ThisNode) {
                spdlog::warn("Node::Disconnect Invalid node");
                return;
            }
            if (ThisNode->NextNode != nullptr) {
                std::erase_if(ThisNode->NextNode->PreviousNodes,
                              [ThisNode](const std::shared_ptr<NodeBase>& NodePtr) {
                                  return NodePtr.get() == ThisNode.get();
                              });
                ThisNode->NextNode = nullptr;
            }
        }
    };

    /**
     * A generic node class for graph algorithms.
     * For now:
     * Nodes can have one outgoing edge to another node. (Outdegree <= 1)
     * Nodes can have multiple incoming edges from other nodes -- with a maximum of 255 (Indegree >= 0 && Indegree <= 255)
     * Essentially, this is a directed graph
     * @tparam DataType The data the node will hold
     */
    template <typename DataType>
    struct Node final : public NodeBase {
        DataType Data{};
    };

    template <typename DataType>
    std::shared_ptr<Node<DataType>> AsNode(const std::shared_ptr<NodeBase>& node) {
        return std::dynamic_pointer_cast<Node<DataType>>(node);
    }

    template <typename DataType>
    Node<DataType>* AsNode(NodeBase* node) {
        return reinterpret_cast<Node<DataType>*>(node);
    }

    inline std::vector<NodeBase*> Flatten(const std::shared_ptr<NodeBase>& RootNode) {
        std::vector<NodeBase*> FlattenedNodes;

        auto hasBeenVisited = [&FlattenedNodes](const std::shared_ptr<NodeBase>& NodePtr) {
            return std::ranges::find(FlattenedNodes, NodePtr.get()) != FlattenedNodes.end();
        };

        std::function<void(std::shared_ptr<NodeBase>)> VisitNode = [&](const std::shared_ptr<NodeBase>& NodePtr) {
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

    inline std::optional<std::vector<NodeBase*>> Kahns(const std::shared_ptr<NodeBase>& RootNode) {
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

        std::vector<NodeBase*> Result;
        auto processNode = [&FlattenedNodes, &InDegreeMap, &Result
            ](size_t Index) -> size_t {
            size_t processCount = 0;
            for (auto i = 0; i < FlattenedNodes.size(); i++) {
                if (InDegreeMap[i] == 0) {
                    Result.push_back(FlattenedNodes[i]);

                    // process the node
                    InDegreeMap[i] = std::numeric_limits<size_t>::max(); // mark as processed
                    processCount++;

                    if (const auto* NodePtr = FlattenedNodes[i]; NodePtr->NextNode) {
                        auto NextIndex = std::ranges::find_if(FlattenedNodes,
                                                              [&NodePtr](const NodeBase* Node) {
                                                                  return Node == NodePtr->NextNode.get();
                                                              });
                        if (NextIndex != FlattenedNodes.end()) {
                            InDegreeMap[std::distance(FlattenedNodes.begin(), NextIndex)] -= 1;
                        }
                    }
                }
            }
            return processCount;
        };

        auto processCount = 0u;
        while (processCount < FlattenedNodes.size()) {
            const auto processed = processNode(processCount);
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
