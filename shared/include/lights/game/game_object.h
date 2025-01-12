//
// Created by ozzadar on 2024-12-31.
//

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace OZZ {
    class GameObject {
    public:
        explicit GameObject();
        virtual ~GameObject() = default;
        virtual void Tick(float DeltaTime) = 0;
    protected:
        glm::vec3 position;
        glm::vec3 scale;
        glm::quat rotation;
    };
} // OZZ