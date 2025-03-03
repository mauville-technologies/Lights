//
// Created by ozzadar on 2025-02-12.
//

#pragma once
#include <lights/game/2D/sprite.h>
#include <lights/scene/scene.h>

namespace OZZ::game::scene {
    class PhysicsTestingLayer : public SceneLayer {
    public:
        explicit PhysicsTestingLayer(GameWorld* inWorld);
        ~PhysicsTestingLayer() override;

        void Init() override;
        void Tick(float DeltaTime) override;
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void RenderTargetResized(glm::ivec2 size) override;
        std::vector<SceneObject> GetSceneObjects() override;
    private:
        GameWorld* world;

        Sprite* pepe;
        uint64_t pepeid;

        Sprite* ground;
        uint64_t groundId;

        Sprite* rightWall;
        uint64_t rightWallId;

        Sprite* leftWall;
        uint64_t leftWallId;

        Sprite* topWall;
        uint64_t topWallId;

        std::shared_ptr<InputSubsystem> input;
    };

} // OZZ