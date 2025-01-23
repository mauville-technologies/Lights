//
// Created by ozzadar on 2025-01-14.
//

#pragma once

#include <glm/glm.hpp>
#include "ozz_collision/ozz_collision_result.h"

namespace OZZ::collision::shapes {
    struct OzzPoint;
    struct OzzCircle {
        glm::vec2 Center;
        float Radius;

        // Circle on Point collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint &other) const;

        // Circle on Circle collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle &other) const;


    };
}
