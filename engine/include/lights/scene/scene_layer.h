//
// Created by ozzadar on 2025-03-22.
//

#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "lights/scene/scene_object.h"
#include <glm/glm.hpp>
#include <vector>

namespace OZZ::scene {
    class Camera {
    public:
        glm::mat4 ViewMatrix{1.f};
        glm::mat4 ProjectionMatrix{1.f};
    };

    class SceneLayer {
    protected:
        friend class SceneLayerManager;

        // Only SceneLayerManager can create/destroy layers
        SceneLayer() = default;

        void SetLayerManager(class SceneLayerManager* manager = nullptr) { layerManager = manager; }

    public:
        virtual ~SceneLayer() = default;

        virtual void Init() {};

        virtual void PhysicsTick(float DeltaTime) {};

        virtual void Tick(float DeltaTime) {};

        virtual void RenderTargetResized(glm::ivec2 size) = 0;

        const Camera& GetCamera() const { return LayerCamera; }

    protected:
        void LookAtPosition2D(const glm::vec2& position) {
            LayerCamera.ViewMatrix = glm::lookAt(glm::vec3{position.x, position.y, 3.f}, // Camera position
                                                 glm::vec3{position.x, position.y, 0.f}, // Target to look at
                                                 glm::vec3{0.f, 1.f, 0.f});              // Up vector
        }

        Camera LayerCamera{};
        SceneLayerManager* layerManager{nullptr};
    };
} // namespace OZZ::scene