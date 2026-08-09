//
// Created by ozzadar on 2025-03-22.
//

#include "lights/framework/scene/scene_layer_manager.h"
#include "lights/framework/layers/clay/clay_ui_layer.h"

#include <algorithm>

namespace OZZ::scene {
    SceneLayerManager::~SceneLayerManager() {
        for (auto& t : asyncLoadingThreads) {
            if (t.joinable())
                t.join();
        }
        activeLayers.clear();
        layerNames.clear();
        layers.clear();
    }

    void SceneLayerManager::InitLayerAsync(const std::string& layerName, rendering::RHIDevice* inDevice) {
        auto* layer = GetLayer<SceneLayer>(layerName);
        if (!layer)
            return;
        asyncLoadingThreads.emplace_back([this, layer, inDevice, layerName]() {
            layer->Init(inDevice);
            if (OnLayerLoaded)
                OnLayerLoaded(layerName);
        });
    }

    void SceneLayerManager::Init(rendering::RHIDevice* inDevice, InputSubsystem* inInput) {
        device = inDevice;
        inputSubsystem = inInput;
        for (const auto& layer : layers) {
            layer->Init(device);
        }
        bIsInitialized = true;
        LoadLayer<ClayUILayer>(device, "ClayUI");
        SetLayerActive("ClayUI", true);
    }

    void SceneLayerManager::RemoveLayer(const std::string& layerName) {
        for (size_t index = 0; index < layerNames.size(); ++index) {
            const auto& name = layerNames[index];
            if (name == layerName) {
                // remove from the active layers if it is there
                erase_if(activeLayers, [index](const auto& layerIndex) {
                    return layerIndex == index;
                });

                // Name freed now; layers[index] stays alive a few more ticks (see
                // RemovalDelayTicks) -- LoadLayer's slot-reuse scan requires both an empty
                // name and a null layer pointer, so this slot is correctly skipped meanwhile.
                layerNames[index] = "";
                pendingRemovals.push_back({index, RemovalDelayTicks});
                bActiveLayersCacheDirty = true;
            }
        }
    }

    void SceneLayerManager::Tick() {
        for (auto& pending : pendingRemovals) {
            --pending.TicksRemaining;
        }
        erase_if(pendingRemovals, [this](const PendingRemoval& pending) {
            if (pending.TicksRemaining > 0)
                return false;
            layers[pending.Index]->DeInit();
            layers[pending.Index].reset();
            return true;
        });
    }

    void SceneLayerManager::SetLayerActive(const std::string& layerName, bool bActive) {
        for (size_t index = 0; index < layerNames.size(); ++index) {
            const auto& name = layerNames[index];
            if (name == layerName) {
                if (bActive) {
                    activeLayers.insert(index);
                } else {
                    activeLayers.erase(index);
                }
                bActiveLayersCacheDirty = true;
                return;
            }
        }
    }

    void SceneLayerManager::SetLayerExecutionOrder(const std::string& layerName, const uint16_t zOrder) {
        for (size_t index = 0; index < layerNames.size(); ++index) {
            const auto& name = layerNames[index];
            if (name == layerName) {
                layerExecutionOrders[index] = zOrder;
                bActiveLayersCacheDirty = true;
            }
        }
    }

    std::vector<SceneLayer*> SceneLayerManager::GetActiveLayers() const {
        if (!bActiveLayersCacheDirty) {
            return activeLayersCache;
        }

        std::vector<size_t> activeLayerIndices(activeLayers.begin(), activeLayers.end());
        std::ranges::sort(activeLayerIndices, [this](const size_t a, const size_t b) {
            return layerExecutionOrders[a] < layerExecutionOrders[b];
        });

        activeLayersCache.clear();
        for (const auto index : activeLayerIndices) {
            if (index < layers.size() && layers[index]) {
                activeLayersCache.push_back(layers[index].get());
            }
        }

        bActiveLayersCacheDirty = false;
        return activeLayersCache;
    }

    std::vector<SceneLayer*> SceneLayerManager::GetAllLayers() const {
        return layers | std::views::transform([](const auto& layer) {
                   return layer.get();
               }) |
               std::ranges::to<std::vector>();
    }
} // namespace OZZ::scene