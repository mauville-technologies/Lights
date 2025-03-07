//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzCircle::IsColliding(const OzzPoint &other) const {
        const auto distance = glm::distance(Position, other.Position);

        return OzzCollisionResult{
            .bCollided = distance <= Radius,
            .ContactPoints = {Position}
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzCircle &other) const {
        const auto distance = glm::distance(Position, other.Position);
        const auto sumRadii = Radius + other.Radius;

        // Get the vector between circles
        const auto vecBetween = other.Position - Position;
        const auto normalizedVec = glm::normalize(vecBetween);
        const auto contactPoint = Position + normalizedVec * Radius;

        return OzzCollisionResult{
            .bCollided = distance <= sumRadii,
            .ContactPoints = {contactPoint}
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzRectangle &other) const {
        glm::vec2 closestPoint = Position;
        const auto [OtherLeft, OtherRight, OtherUp, OtherDown] = other.GetExtents();
        closestPoint = glm::clamp(closestPoint, glm::vec2{OtherLeft, OtherDown}, glm::vec2{OtherRight, OtherUp});

        const glm::vec2 vecBetween = Position - closestPoint;
        const float distance = glm::length(vecBetween);
        glm::vec2 penetrationDepth = glm::normalize(vecBetween) * (Radius - distance);
        if (distance <= Radius) {
            return {
                .bCollided = true,
                .ContactPoints = {closestPoint},
                .CollisionNormal = penetrationDepth,
            };
        }
        return OzzCollisionResult::NoCollision();
    }
}
