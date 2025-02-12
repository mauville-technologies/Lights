//
// Created by ozzadar on 2024-12-31.
//

#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ozz_collision/world.h>

namespace OZZ {
    class GameObject {
    public:
        explicit GameObject(std::shared_ptr<OzzWorld2D>);
        virtual ~GameObject() = default;
        virtual void Tick(float DeltaTime) = 0;
    protected:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;

        std::shared_ptr<OzzWorld2D> collisionSystem;
    };
} // OZZ