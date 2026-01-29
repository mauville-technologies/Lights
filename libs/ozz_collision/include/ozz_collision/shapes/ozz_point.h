//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include "ozz_collision/ozz_collision_result.h"
#include <glm/glm.hpp>

namespace OZZ::collision::shapes {
    struct OzzCircle;
    struct OzzRectangle;
    struct OzzLine;

    struct OzzPoint {
        glm::vec2 Position;

        glm::vec2 Scale;

        // Point on Point
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint& other) const;

        // Point on Circle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle& other) const;

        // Point on Rectangle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle& other) const;

        // Point on Line
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine& other) const;
    };
} // namespace OZZ::collision::shapes
