//
// Created by ozzadar on 2025-03-22.
//

#include "lights/framework/scene/scene_layer_manager.h"

#include <algorithm>

namespace OZZ::scene {
    SceneLayerManager::~SceneLayerManager() {
        activeLayers.clear();
        layerNames.clear();
        layers.clear();
    }

    void SceneLayerManager::Init() {
        // initialize all layers
        for (const auto& layer : layers) {
            layer->Init();
        }

        bIsInitialized = true;
    }

    void SceneLayerManager::RemoveLayer(const std::string& layerName) {
        for (const auto& [index, name] : layerNames | std::ranges::views::enumerate) {
            if (name == layerName) {
                // remove from the active layers if it is there
                erase_if(activeLayers, [index](const auto& layerIndex) {
                    return layerIndex == index;
                });

                // remove the layer from the names and layers by emptying the slot. We keep the slots empty for re-use
                // to avoid invalidating the indices
                layerNames[index] = "";
                layers[index]->DeInit();
                layers[index].reset();
            }
        }
    }

    void SceneLayerManager::SetLayerActive(const std::string& layerName, bool bActive) {
        for (const auto& [index, name] : layerNames | std::ranges::views::enumerate) {
            if (name == layerName) {
                if (bActive) {
                    activeLayers.insert(index);
                } else {
                    activeLayers.erase(index);
                }
                return;
            }
        }
    }

    void SceneLayerManager::SetLayerExecutionOrder(const std::string& layerName, const uint16_t zOrder) {
        for (const auto& [index, name] : layerNames | std::ranges::views::enumerate) {
            if (name == layerName) {
                layerExecutionOrders[index] = zOrder;
            }
        }
    }

    std::vector<SceneLayer*> SceneLayerManager::GetActiveLayers() const {
        std::vector<size_t> activeLayerIndices{};

        // list the active layers
        for (const auto index : activeLayers) {
            activeLayerIndices.push_back(index);
        }

        std::ranges::sort(activeLayerIndices, [this](const size_t a, const size_t b) {
            return layerExecutionOrders[a] < layerExecutionOrders[b];
        });

        std::vector<SceneLayer*> result;
        for (const auto index : activeLayerIndices) {
            if (index < layers.size() && layers[index]) {
                result.push_back(this->layers[index].get());
            }
        }
        return result;
    }

    std::vector<SceneLayer*> SceneLayerManager::GetAllLayers() const {
        return layers | std::views::transform([](const auto& layer) {
                   return layer.get();
               }) |
               std::ranges::to<std::vector>();
    }
} // namespace OZZ::scene