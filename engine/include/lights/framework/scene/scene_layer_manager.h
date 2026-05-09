//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include <cassert>
#include <functional>
#include <ranges>
#include <thread>

#include <lights/framework/scene/scene_layer.h>
#include <set>
#include <string>

namespace OZZ::scene {
    class SceneLayerManager {
        friend class SceneLayer;

    public:
        ~SceneLayerManager();
        void Init(rendering::RHIDevice* inDevice);

        // Delegates — set these before calling InitLayerAsync
        std::function<void(const std::string& layerName, float progress, std::string statusText)> OnLayerProgress;
        std::function<void(const std::string& layerName)> OnLayerLoaded;

        template <typename T, typename... Args>
        T* LoadLayer(rendering::RHIDevice* inDevice, const std::string& layerName, Args&&... args) {
            for (const auto& [index, name] : layerNames | std::ranges::views::enumerate) {
                if (name != layerName) {
                    continue;
                }

                auto* existingLayer = dynamic_cast<T*>(layers[index].get());
                assert(existingLayer && "Layer name already exists with a different type.");
                return existingLayer;
            }

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
            insertedLayer->SetLayerName(layerName);

            // if the manager is already initialized, we initialize the new layer
            if (bIsInitialized) {
                insertedLayer->Init(inDevice);
            }
            return insertedLayer;
        }

        // Allocates the layer slot and sets it up, but does NOT call Init().
        // Caller must call InitLayerAsync() or layer->Init() manually.
        template <typename T, typename... Args>
        T* LoadLayerDeferred(const std::string& layerName, Args&&... args) {
            for (const auto& [index, name] : layerNames | std::ranges::views::enumerate) {
                if (name != layerName) continue;
                auto* existingLayer = dynamic_cast<T*>(layers[index].get());
                assert(existingLayer && "Layer name already exists with a different type.");
                return existingLayer;
            }

            bool wasInserted = false;
            auto newLayer = std::make_unique<T>(std::forward<Args>(args)...);
            for (size_t i = 0; i < layers.size(); ++i) {
                if (layerNames[i].empty() && !layers[i]) {
                    layers[i] = std::move(newLayer);
                    layerNames[i] = layerName;
                    wasInserted = true;
                    break;
                }
            }
            if (!wasInserted) {
                layers.emplace_back(std::move(newLayer));
                layerNames.push_back(layerName);
                layerExecutionOrders.emplace_back(0);
            }

            auto insertedLayer = GetLayer<T>(layerName);
            insertedLayer->SetLayerManager(this);
            insertedLayer->SetLayerName(layerName);
            return insertedLayer;
        }

        // Spawns a background thread that calls Init() on the named layer.
        // Fires OnLayerLoaded when complete. Layer must have been registered via
        // LoadLayerDeferred first.
        void InitLayerAsync(const std::string& layerName, rendering::RHIDevice* inDevice);

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
        void NotifyLayerProgress(const std::string& layerName, float progress, std::string statusText) {
            if (OnLayerProgress) OnLayerProgress(layerName, progress, std::move(statusText));
        }

        rendering::RHIDevice* device{nullptr};
        std::set<size_t> activeLayers;
        std::vector<std::string> layerNames;
        std::vector<uint16_t> layerExecutionOrders;
        std::vector<std::unique_ptr<SceneLayer>> layers;
        std::vector<std::thread> asyncLoadingThreads;

        bool bIsInitialized = false;
    };
} // namespace OZZ::scene
