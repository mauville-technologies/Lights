//
// Created by ozzadar on 2025-02-12.
//

#pragma once
#include <game/objects/pepe.h>
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

        Pepe* pepe;
        uint64_t pepeid;
    };

} // OZZ