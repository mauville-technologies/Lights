//
// Created by ozzadar on 2025-01-01.
//

#pragma once
#include "lights/game/game_object.h"
#include "lights/scene/scene.h"

namespace OZZ {

    class GroundTest : public GameObject {
    public:
        explicit GroundTest(std::shared_ptr<CollisionSystem> InCollision);
        void Tick(float DeltaTime) override;

        SceneObject* GetSceneObject() { return &sceneObject; }
    private:
        SceneObject sceneObject;
    };

} // OZZ
