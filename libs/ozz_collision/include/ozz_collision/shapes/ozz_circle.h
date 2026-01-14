//
// Created by ozzadar on 2025-01-14.
//

#pragma once

#include "ozz_collision/ozz_collision_result.h"
#include <glm/glm.hpp>

namespace OZZ::collision::shapes {
    struct OzzPoint;
    struct OzzRectangle;
    struct OzzLine;

    struct OzzCircle {
        glm::vec2 Position;
        float Radius;

        glm::vec2 Scale() const { return glm::vec2{Radius * 2, Radius * 2}; }

        void SetScale(const glm::vec2 scale) { Radius = scale.x; }

        // Circle on Point collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint& other) const;

        // Circle on Circle collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle& other) const;

        // Circle on Rectangle collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle& other) const;

        // Circle on Line collision
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine& other) const;
    };
} // namespace OZZ::collision::shapes
