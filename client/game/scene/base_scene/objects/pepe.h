//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include "lights/scene/scene.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    class Pepe : public GameObject {
    public:
        explicit Pepe(std::shared_ptr<CollisionSystem> InCollision);
        ~Pepe() override;

        void Tick(float DeltaTime) override;

        void Jump();
        SceneObject* GetSceneObject() { return &sceneObject; }

        void MoveLeft();
        void MoveRight();
        void StopMoving();

    private:
        SceneObject sceneObject;
    };
}
