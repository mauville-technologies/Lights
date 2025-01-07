//
// Created by ozzadar on 2024-12-19.
//

#pragma once

#include "lights/scene/scene.h"
#include "lights/game/game_object.h"

namespace OZZ::game::scene {
    class Pepe : public GameObject {
    public:
        explicit Pepe(b2WorldId worldId);
        ~Pepe();

        void Tick(float DeltaTime) override;

        void Jump();
        SceneObject* GetSceneObject() { return &sceneObject; }
    private:
        SceneObject sceneObject;
        b2BodyId bodyId;
    };
}
