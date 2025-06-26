//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "lights/scene/scene_object.h"

namespace OZZ::scene {
    class Camera {
    public:
        glm::mat4 ViewMatrix{1.f};
        glm::mat4 ProjectionMatrix{1.f};
    };

    class SceneLayer {
    public:
        virtual ~SceneLayer() = default;

        virtual void Init() {
        };

        virtual void PhysicsTick(float DeltaTime) {
        };

        virtual void Tick(float DeltaTime) {
        };

        virtual void RenderTargetResized(glm::ivec2 size) = 0;

        virtual std::vector<SceneObject> GetSceneObjects() = 0;

        Camera LayerCamera{};
    };
}