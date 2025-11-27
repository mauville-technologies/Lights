//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include <ranges>

#include <lights/scene/scene_layer.h>
#include <set>
#include <string>

namespace OZZ::scene {
    class SceneLayerManager {
    public:
        ~SceneLayerManager();
        void Init();

        template <typename T, typename... Args>
        T* LoadLayer(const std::string& layerName, Args&&... args) {
            bool wasInserted = false;
            auto newLayer = std::make_unique<T>(std::forward<Args>(args)...);
            // First we look for an empty slot to insert the layer
            for (size_t i = 0; i < layers.size(); ++i) {
                auto& layer = layers[i];
                auto& name = layerNames[i];

                if (name.empty() && !layer) {
                    layer = std::move(newLayer);
                    name = layerName;
                    wasInserted = true;
                    break;
                }
            }

            if (!wasInserted) {
                layers.emplace_back(std::move(newLayer));
                layerNames.push_back(layerName);
                layerExecutionOrders.emplace_back(0); // default z-order
            }

            auto insertedLayer = GetLayer<T>(layerName);
            insertedLayer->SetLayerManager(this);

            // if the manager is already initialized, we initialize the new layer
            if (bIsInitialized) {
                insertedLayer->Init();
            }
            return insertedLayer;
        }

        void RemoveLayer(const std::string& layerName);

        template <typename LayerType>
        LayerType* GetLayer(const std::string& layerName) {
            SceneLayer* layer{nullptr};
            for (const auto& [index, name] : this->layerNames | std::ranges::views::enumerate) {
                if (name == layerName) {
                    layer = layers[index].get();
                    break;
                }
            }

            return dynamic_cast<LayerType*>(layer);
        }

        void SetLayerActive(const std::string& layerName, bool bActive);
        void SetLayerExecutionOrder(const std::string& layerName, uint16_t zOrder);

        [[nodiscard]] std::vector<SceneLayer*> GetActiveLayers() const;
        std::vector<SceneLayer*> GetAllLayers() const;

    private:
        std::set<size_t> activeLayers;
        std::vector<std::string> layerNames;
        std::vector<uint16_t> layerExecutionOrders;
        std::vector<std::unique_ptr<SceneLayer>> layers;

        bool bIsInitialized = false;
    };
} // namespace OZZ::scene
