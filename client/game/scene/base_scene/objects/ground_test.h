//
// Created by ozzadar on 2025-01-01.
//

#pragma once
#include "lights/game/game_object.h"
#include "lights/scene/scene.h"

namespace OZZ {

    class GroundTest : public GameObject {
    public:
        explicit GroundTest(b2WorldId worldId);
        void Tick(float DeltaTime) override;

        SceneObject* GetSceneObject() { return &sceneObject; }
    private:
        SceneObject sceneObject;

        // Physics things
        b2BodyId bodyId;
    };

} // OZZ
