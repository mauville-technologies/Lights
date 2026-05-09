//
// Created by ozzadar on 2025-03-22.
//

#include "lights/framework/scene/scene_layer.h"
#include "lights/framework/scene/scene_layer_manager.h"

namespace OZZ::scene {
    void SceneLayer::SetLoadingProgress(float progress, std::string statusText) {
        if (layerManager) layerManager->NotifyLayerProgress(layerName, progress, std::move(statusText));
    }
} // namespace OZZ::scene
