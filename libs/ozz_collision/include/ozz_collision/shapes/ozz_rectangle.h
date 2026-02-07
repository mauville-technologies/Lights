//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include "ozz_collision/ozz_collision_result.h"
#include <glm/glm.hpp>

namespace OZZ::collision::shapes {
    struct OzzCircle;
    struct OzzPoint;
    struct OzzLine;

    struct OzzRectangle {
        struct Extents {
            float Left;
            float Right;
            float Up;
            float Down;
        };

        glm::vec2 Position;
        glm::vec2 Size;

        glm::vec2 Scale;

        // Point on Point
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint& other) const;

        // Point on Circle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle& other) const;

        // Rectangle on Rectangle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle& other) const;

        // Rectange on Line
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine& other) const;

        [[nodiscard]] glm::vec2 GetScaledSize() const;

        [[nodiscard]] Extents GetExtents() const;
    };
} // namespace OZZ::collision::shapes
