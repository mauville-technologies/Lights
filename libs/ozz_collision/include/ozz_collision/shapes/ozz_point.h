//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include <glm/glm.hpp>
#include "ozz_collision/ozz_collision_result.h"
#include "ozz_collision/shapes/ozz_circle.h"

namespace OZZ::collision::shapes {
    struct OzzCircle;

    struct OzzPoint {
        glm::vec2 Position;

        // Point on Point
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint &other) const;

        // Point on Circle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle &other) const;
    };
}
