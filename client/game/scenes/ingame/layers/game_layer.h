//
// Created by ozzadar on 2025-02-11.
//

#pragma once
#include <lights/scene/scene.h>
#include <lights/game/world.h>
#include <lights/input/input_subsystem.h>
#include "game/objects/pepe.h"
#include "game/objects/tilemap.h"

namespace OZZ::game::scene {
    class GameLayer : public SceneLayer {
    public:
        explicit GameLayer(World* inWorld);
        ~GameLayer() override;
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;

        std::vector<SceneObject> GetSceneObjects() override;
    private:
        void unregisterMappings(std::shared_ptr<InputSubsystem> inInput);
        void registerMappings(std::shared_ptr<InputSubsystem> inInput);
    private:
        std::pair<uint64_t, Pepe*> pepe {0, nullptr};
        std::pair<uint64_t, Tilemap*> tilemap {0, nullptr};

        std::shared_ptr<InputSubsystem> input;

        World* world;
    };
} // OZZ
