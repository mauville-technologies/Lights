//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include "ozz_collision/ozz_collision_result.h"
#include <glm/glm.hpp>

namespace OZZ::collision::shapes {
    struct OzzCircle;
    struct OzzRectangle;
    struct OzzPoint;

    struct OzzLine {
        glm::vec2 Position;
        glm::vec2 End;

        glm::vec2 Scale;

        // Line on Line
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine& other) const;

        // Point on Point
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint& other) const;

        // Point on Circle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle& other) const;

        // Point on Rectangle
        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle& other) const;

        // Helper to compute scaled endpoints from center
        static void GetScaledEndpoints(const glm::vec2& position,
                                       const glm::vec2& end,
                                       const glm::vec2& scale,
                                       glm::vec2& outStart,
                                       glm::vec2& outEnd) {
            const glm::vec2 center = (position + end) * 0.5f;
            const glm::vec2 halfExtent = (end - position) * 0.5f * scale;
            outStart = center - halfExtent;
            outEnd = center + halfExtent;
        }
    };
} // namespace OZZ::collision::shapes
