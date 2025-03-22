//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include <string>
#include "lights/scene/scene_layer.h"

namespace OZZ::scene {
	class SceneLayerManager {
	public:
		void AddLayer();
		SceneLayer* GetLayer(const std::string& LayerName);
		void RemoveLayer(const std::string& LayerName);
	private:

	};
} // OZZ
