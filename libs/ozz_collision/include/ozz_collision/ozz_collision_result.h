//
// Created by ozzadar on 2025-01-14.
//

#pragma once
#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace OZZ::collision {
    // TODO: If I add more shapes, I might need more information. If I start handling physics, I'll need to add more
    // information
    struct OzzCollisionResult {
        static constexpr uint8_t MaxContactPoints = 8;

        bool bCollided{false};
        std::array<glm::vec2, MaxContactPoints> ContactPoints{};
        uint8_t ContactPointCount{0};
        glm::vec2 CollisionNormal{};
        float PenetrationDepth{0.f};

        static OzzCollisionResult NoCollision() { return {.bCollided = false}; }

        void AddContactPoint(glm::vec2 point) {
            if (ContactPointCount < MaxContactPoints) {
                ContactPoints[ContactPointCount++] = point;
            }
        }
    };

    template <typename Collider, typename Collidee>
    OzzCollisionResult IsColliding(const Collider& collider, const Collidee& collidee) {
        return collider.IsColliding(collidee);
    }
} // namespace OZZ::collision