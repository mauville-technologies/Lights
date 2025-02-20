//
// Created by ozzadar on 2025-01-14.
//

#pragma once

#include <glm/glm.hpp>
#include "ozz_collision/ozz_collision_result.h"

namespace OZZ::collision::shapes {
    struct OzzPoint;
    struct OzzRectangle;

    struct OzzCircle {
        glm::vec2 Position;
        float Radius;

        // Circle on Point collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint &other) const;

        // Circle on Circle collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle &other) const;

        // Circle on Rectangle collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle &other) const;
    };
}
