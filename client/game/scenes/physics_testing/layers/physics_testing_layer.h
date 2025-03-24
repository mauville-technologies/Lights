//
// Created by ozzadar on 2025-02-12.
//

#pragma once
#include <lights/game/2D/sprite.h>
#include <lights/scene/scene.h>

namespace OZZ::game::objects {
    class Player;
}

namespace OZZ::game::scene {
    class PhysicsTestingLayer : public OZZ::scene::SceneLayer {
    public:
        explicit PhysicsTestingLayer(GameWorld* inWorld);
        ~PhysicsTestingLayer() override;

        void Init() override;
        void PhysicsTick(float DeltaTime) override;
        void Tick(float DeltaTime) override;
        void SetInputSubsystem(const std::shared_ptr<InputSubsystem>& inInput);
        void RenderTargetResized(glm::ivec2 size) override;
        std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;
    private:
        void updateViewMatrix();
    private:
        GameWorld* gameWorld;

        std::pair<uint64_t, objects::Player*> pepe {UINT64_MAX, nullptr};

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