//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_circle.h"

#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    // Helper to compute scaled radius (use max of scale components for uniform circle)
    static float GetScaledRadius(float radius, const glm::vec2& scale) {
        return radius * glm::max(scale.x, scale.y);
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzPoint& other) const {
        const float scaledRadius = GetScaledRadius(Radius, Scale);
        const auto vecBetween = other.Position - Position;
        const auto distance = glm::distance(Position, other.Position);
        if (distance <= scaledRadius) {
            glm::vec2 collisionNormal = (distance > 0.0001f) ? glm::normalize(vecBetween) // Unit vector, not scaled
                                                             : glm::vec2{0.f, 1.f};
            const float penetrationDepth = scaledRadius - distance;

            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {Position},
                .CollisionNormal = collisionNormal,
                .PenetrationDepth = penetrationDepth,
            };
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzCircle& other) const {
        const float scaledRadius = GetScaledRadius(Radius, Scale);
        const float otherScaledRadius = GetScaledRadius(other.Radius, other.Scale);
        const auto distance = glm::distance(Position, other.Position);
        const auto sumRadii = scaledRadius + otherScaledRadius;

        // Get the vector between circles
        const auto vecBetween = other.Position - Position;
        const auto normalizedVec = glm::normalize(vecBetween);
        const auto contactPoint = Position + normalizedVec * scaledRadius;
        const auto penetrationDepth = sumRadii - distance;

        return OzzCollisionResult{
            .bCollided = distance <= sumRadii,
            .ContactPoints = {contactPoint},
            .CollisionNormal = normalizedVec,
            .PenetrationDepth = penetrationDepth,
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzRectangle& other) const {
        const float scaledRadius = GetScaledRadius(Radius, Scale);
        glm::vec2 closestPoint = Position;
        const auto [OtherLeft, OtherRight, OtherUp, OtherDown] = other.GetExtents();
        closestPoint = glm::clamp(closestPoint, glm::vec2{OtherLeft, OtherDown}, glm::vec2{OtherRight, OtherUp});

        const glm::vec2 vecBetween = Position - closestPoint;
        const float distance = glm::length(vecBetween);
        if (distance <= scaledRadius) {
            const glm::vec2 collisionNormal = (distance > 0.0001f)
                                                  ? glm::normalize(vecBetween) // Unit vector, not scaled
                                                  : glm::vec2{0.f, 1.f};
            const float penetrationDepth = scaledRadius - distance;
            return {.bCollided = true,
                    .ContactPoints = {closestPoint},
                    .CollisionNormal = collisionNormal,
                    .PenetrationDepth = penetrationDepth};
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzLine& other) const {
        return other.IsColliding(*this);
    }
} // namespace OZZ::collision::shapes
