//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include <glm/glm.hpp>
#include "ozz_collision/ozz_collision_result.h"

namespace OZZ::collision::shapes {
    struct OzzCircle;
    struct OzzRectangle;
    struct OzzPoint;

    struct OzzLine {
        glm::vec2 Position;
        glm::vec2 End;

        glm::vec2 Scale() const {
            return {1.f, 1.f};
        }
        //Line on Line
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine &other) const;

        // Point on Point
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint &other) const;

        // Point on Circle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle &other) const;

        // Point on Rectangle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle &other) const;
    };
}
