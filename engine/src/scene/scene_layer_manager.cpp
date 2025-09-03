//
// Created by ozzadar on 2025-03-22.
//

#include "lights/scene/scene_layer_manager.h"

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

    std::vector<SceneLayer*> SceneLayerManager::GetActiveLayers() const {
        std::vector<SceneLayer*> result;
        for (const auto index : activeLayers) {
            if (index < layers.size()) {
                result.push_back(this->layers[index].get());
            }
        }
        return result;
    }
} // namespace OZZ::scene