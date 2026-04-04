//
// Created by Claude on 2026-04-04.
//

#pragma once

#include <cstddef>
#include <glm/glm.hpp>

namespace OZZ::collision::raycast {
    struct OzzRaycastResult3D {
        bool bHit{false};
        float Distance{0.f};            // parametric t along the ray
        glm::vec3 HitPoint3D{};         // world-space intersection point
        glm::vec2 HitPointLocal2D{};    // hit point in the shape's local 2D space
        glm::vec3 PlaneNormal{};        // normal of the plane that was hit

        static OzzRaycastResult3D NoHit() { return {.bHit = false}; }
    };

    struct OzzRaycastHit3D {
        OzzRaycastResult3D Result;
        size_t TargetIndex{};            // index into the targets array
    };
} // namespace OZZ::collision::raycast
