//
// Created by paulm on 2026-04-02.
//

#pragma once

#include "ozz_collision/ozz_collision_result.h"
#include <glm/glm.hpp>

namespace OZZ::collision::shapes {
    struct OzzCircle;
    struct OzzRectangle;
    struct OzzLine;
    struct OzzPoint;

    struct OzzPolygon {
        glm::vec2 Position{0.f, 0.f};
        glm::vec2 Scale{1.f, 1.f};
        std::vector<glm::vec2> Vertices;

        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPoint& other) const;

        [[nodiscard]] OzzCollisionResult IsColliding(const OzzCircle& other) const;

        [[nodiscard]] OzzCollisionResult IsColliding(const OzzRectangle& other) const;

        [[nodiscard]] OzzCollisionResult IsColliding(const OzzLine& other) const;

        [[nodiscard]] OzzCollisionResult IsColliding(const OzzPolygon& other) const;
    };
} // namespace OZZ::collision::shapes
