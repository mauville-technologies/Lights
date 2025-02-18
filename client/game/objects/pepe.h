//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include "lights/scene/scene.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    class Pepe : public GameObject {
    public:
        explicit Pepe(std::shared_ptr<OzzWorld2D> InCollision);
        ~Pepe() override;

        void Tick(float DeltaTime) override;

        void Jump();
        std::vector<SceneObject> GetSceneObjects() override { return {sceneObject}; }

        void MoveLeft();
        void MoveRight();
        void StopMoving();

        SceneObject& GetSceneObjectRef() { return sceneObject; };

    private:
        SceneObject sceneObject;
    };
}
