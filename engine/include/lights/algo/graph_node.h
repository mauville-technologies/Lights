//
// Created by ozzadar on 2025-10-19.
//

#pragma once
#include <memory>
#include <optional>
#include <vector>

class GraphNode {
public:
    /**
     * Create directed connection between two nodes
     * @param from source node
     * @param to target node
     */
    static void Connect(GraphNode *from, GraphNode *to);

    /**
     * Removes directed connection between to nodes
     * @param from source node
     * @param to target node
     */
    static void Disconnect(GraphNode *from, GraphNode *to);

    /**
     * Perform a topological sort of the subgraph reachable from the given root.
     * Ensures that each node appears after all of its dependencies (inputs).
     *
     * @param root  starting node from which the reachable subgraph is analyzed
     * @return      vector of nodes in valid dependency order for execution
     */
    static std::optional<std::vector<GraphNode *> > TopologicalSort(GraphNode *root);

    /**
     * Flatten node graph into an unsorted list of nodes
     * @return unsorted list of nodes
     */
    static std::vector<GraphNode *> Flatten(GraphNode *root);

    [[nodiscard]] size_t GetInDegree() const { return inputs.size(); }

    [[nodiscard]] size_t GetOutDegree() const { return outputs.size(); }

    static bool AreConnected(GraphNode *from, GraphNode *to);

    static void ClearConnections(GraphNode *node);

protected:
    std::vector<GraphNode *> outputs{};
    std::vector<GraphNode *> inputs{};
};

//
// template <typename Interface, typename BaseInterface = void>
// class Node : public std::conditional_t<!std::is_void_v<BaseInterface>, Node<BaseInterface>, NodeBase> {
// public:
//     template <typename... Args>
//     Node(Args&&... args) {
//         data = std::make_unique<Interface>(std::forward<Args>(args)...);
//     }
//
//     Interface* Data() { return data.get(); }
//
// private:
//     std::unique_ptr<Interface> data;
// };
