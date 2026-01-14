//
// Created by ozzadar on 2025-10-26.
//
#include "lights/scene/scene.h"

namespace OZZ {
    void scene::Scene::InitScene(std::shared_ptr<InputSubsystem> inInput, ResourceManager* inResourceManager) {
        input = std::move(inInput);
        layerManager = std::make_unique<SceneLayerManager>();
        resourceManager = inResourceManager;
    }

    void scene::Scene::Tick(float DeltaTime) {
        static constexpr float physicsTickRate = 1.f / 60.f;
        static float accumulator = 0.f;
        accumulator += DeltaTime;

        const auto& activeLayers = GetActiveLayers();
        while (accumulator >= physicsTickRate) {
            for (const auto& Layer : activeLayers) {
                Layer->PhysicsTick(physicsTickRate);
            }

            // TODO: @paulm - Physics tick
            accumulator -= physicsTickRate;
        }
        for (const auto& Layer : activeLayers) {
            Layer->Tick(DeltaTime);
        }
    }

    void scene::Scene::WindowResized(const glm::ivec2 size) {
        // the render targets should resize even if they're not active so that when they become active again they're
        // correct
        for (const auto& Layer : GetAllLayers()) {
            if (!Layer)
                // when a layer is removed, we don't shrink the array in order to preserve index order
                continue;
            Layer->RenderTargetResized(size);
        }
    }

    scene::Scene::~Scene() {
        layerManager.reset();
    }
} // namespace OZZ