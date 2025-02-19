//
// Created by ozzadar on 2025-02-12.
//

#pragma once
#include <lights/game/2D/sprite.h>
#include <lights/scene/scene.h>

namespace OZZ::game::scene {
    class PhysicsTestingLayer : public SceneLayer {
    public:
        explicit PhysicsTestingLayer(World* inWorld);
        ~PhysicsTestingLayer() override;

        void Init() override;
        void Tick(float DeltaTime) override;
        void RenderTargetResized(glm::ivec2 size) override;
        std::vector<SceneObject> GetSceneObjects() override;
    private:
        World* world;

        Sprite* pepe;
        Sprite* pepe2;
        uint64_t pepeid;
        uint64_t pepe2id;
    };

} // OZZ