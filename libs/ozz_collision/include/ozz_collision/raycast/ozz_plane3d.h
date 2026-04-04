//
// Created by Claude on 2026-04-04.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace OZZ::collision::raycast {
    struct OzzPlane3D {
        glm::vec3 Origin{0.f};              // shape's local (0,0) in world space
        glm::vec3 Normal{0.f, 1.f, 0.f};    // plane normal (unit vector)
        glm::vec3 Right{1.f, 0.f, 0.f};     // local +X axis in 3D
        glm::vec3 Up{0.f, 0.f, -1.f};       // local +Y axis in 3D

        static OzzPlane3D FromPositionAndOrientation(const glm::vec3& position, const glm::quat& orientation);
        static OzzPlane3D FromModelMatrix(const glm::mat4& model);
    };
} // namespace OZZ::collision::raycast
