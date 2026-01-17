//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_circle.h"

#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzCircle::IsColliding(const OzzPoint& other) const {
        const auto vecBetween = other.Position - Position;
        const auto distance = glm::distance(Position, other.Position);
        if (distance <= Radius) {
            glm::vec2 collisionNormal = (distance > 0.0001f) ? glm::normalize(vecBetween) // Unit vector, not scaled
                                                             : glm::vec2{0.f, 1.f};
            const float penetrationDepth = Radius - distance;

            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {Position},
                .CollisionNormal = collisionNormal,
                .PenetrationDepth = penetrationDepth,
            };
        }
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzCircle& other) const {
        const auto distance = glm::distance(Position, other.Position);
        const auto sumRadii = Radius + other.Radius;

        // Get the vector between circles
        const auto vecBetween = other.Position - Position;
        const auto normalizedVec = glm::normalize(vecBetween);
        const auto contactPoint = Position + normalizedVec * Radius;
        const auto penetrationDepth = sumRadii - distance;

        return OzzCollisionResult{
            .bCollided = distance <= sumRadii,
            .ContactPoints = {contactPoint},
            .CollisionNormal = normalizedVec,
            .PenetrationDepth = penetrationDepth,
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzRectangle& other) const {
        glm::vec2 closestPoint = Position;
        const auto [OtherLeft, OtherRight, OtherUp, OtherDown] = other.GetExtents();
        closestPoint = glm::clamp(closestPoint, glm::vec2{OtherLeft, OtherDown}, glm::vec2{OtherRight, OtherUp});

        const glm::vec2 vecBetween = Position - closestPoint;
        const float distance = glm::length(vecBetween);
        if (distance <= Radius) {
            const glm::vec2 collisionNormal = (distance > 0.0001f)
                                                  ? glm::normalize(vecBetween) // Unit vector, not scaled
                                                  : glm::vec2{0.f, 1.f};
            const float penetrationDepth = Radius - distance;
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
