//
// Created by ozzadar on 2025-03-22.
//

#include "lights/scene/scene_layer_manager.h"
#include <ranges>


namespace OZZ::scene {
	SceneLayerManager::~SceneLayerManager() {
		ActiveLayers.clear();
		LayerNames.clear();
		Layers.clear();
	}

	void SceneLayerManager::Init() {
		// initialize all layers
		for (const auto& layer : Layers) {
			layer->Init();
		}

		bIsInitialized = true;
	}

	void SceneLayerManager::RemoveLayer(const std::string &LayerName) {
		for (const auto& [index, name] : LayerNames | std::ranges::views::enumerate) {
			if (name == LayerName) {
				// remove from the active layers if it is there
				erase_if(ActiveLayers, [index](const auto& layerIndex) { return layerIndex == index; });

				// remove the layer from the names and layers by emptying the slot. We keep the slots empty for re-use to
				// avoid invalidating the indices
				LayerNames[index] = "";
				Layers[index].reset();
			}
		}
	}

	SceneLayer * SceneLayerManager::GetLayer(const std::string &LayerName) {
		for (const auto& [index, name] : LayerNames | std::ranges::views::enumerate) {
			if (name == LayerName) {
				return Layers[index].get();
			}
		}
		return nullptr;
	}

	void SceneLayerManager::SetLayerActive(const std::string &LayerName, bool active) {
		for (const auto& [index, name] : LayerNames | std::ranges::views::enumerate) {
			if (name == LayerName) {
				if (active) {
					ActiveLayers.insert(index);
				} else {
					ActiveLayers.erase(index);
				}
				return;
			}
		}
	}

	std::vector<SceneLayer *> SceneLayerManager::GetActiveLayers() const {
		// use ranges library to pick out the active layers
		std::vector<SceneLayer *> layers;
		for (const auto index : ActiveLayers) {
			if (index < Layers.size()) {
				layers.push_back(Layers[index].get());
			}
		}
		return layers;
	}
} // OZZ