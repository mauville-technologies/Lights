//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include <string>
#include <set>
#include <ranges>
#include <lights/scene/scene_layer.h>

namespace OZZ::scene {
	class SceneLayerManager {
	public:
		~SceneLayerManager();
		void Init();

		template <typename T, typename... Args>
		T* LoadLayer(const std::string& layerName, Args&&... args) {;
			bool wasInserted = false;
			auto newLayer = std::make_unique<T>(std::forward<Args>(args)...);
			// First we look for an empty slot to insert the layer
			for (size_t i = 0; i < Layers.size(); ++i) {
				auto& layer = Layers[i];
				auto& name = LayerNames[i];

				if (name.empty() && !layer) {
					layer = std::move(newLayer);
					name = layerName;
					wasInserted = true;
					break;
				}
			}

			if (!wasInserted) {
				Layers.emplace_back(std::move(newLayer));
				LayerNames.push_back(layerName);
			}

			auto insertedLayer = dynamic_cast<T*>(GetLayer(layerName));
			// if the manager is already initialized, we initialize the new layer
			if (bIsInitialized) {
				insertedLayer->Init();
			}
			return insertedLayer;
        }
		void RemoveLayer(const std::string& LayerName);
		SceneLayer* GetLayer(const std::string& LayerName);

		void SetLayerActive(const std::string& LayerName, bool active);

		[[nodiscard]] std::vector<SceneLayer*> GetActiveLayers() const;


	private:
		std::set<size_t> ActiveLayers;
		std::vector<std::string> LayerNames;
        std::vector<std::unique_ptr<SceneLayer>> Layers;

		bool bIsInitialized = false;
	};
} // OZZ
