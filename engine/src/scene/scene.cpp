//
// Created by ozzadar on 2025-10-26.
//
#include "lights/scene/scene.h"

namespace OZZ {
    void scene::Scene::InitScene(std::shared_ptr<InputSubsystem> inInput) {
        input = std::move(inInput);
        layerManager = std::make_unique<SceneLayerManager>();

        const auto theWorld = GetWorld();
        theWorld->Init({
            .Gravity = {0.f, -20.f * game::constants::PhysicsUnitPerMeter},
        });
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
            GetWorld()->PhysicsTick(physicsTickRate);
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
            Layer->RenderTargetResized(size);
        }
    }

    scene::Scene::~Scene() {
        layerManager.reset();

        if (world)
            world->DeInit();
        world.reset();
    }

    GameWorld* scene::Scene::GetWorld() {
        if (!world)
            world = std::make_shared<GameWorld>();
        return world.get();
    }
} // namespace OZZ