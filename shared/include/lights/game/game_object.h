//
// Created by ozzadar on 2024-12-31.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <box2d/box2d.h>

namespace OZZ {
    class GameObject {
    public:
        explicit GameObject(b2WorldId worldId);

        virtual void Tick(float DeltaTime) = 0;
    protected:
        b2WorldId worldId;
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;
    };
} // OZZ