//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzCircle::IsColliding(const OzzPoint &other) const {
        const auto distance = glm::distance(Center, other.Position);

        return OzzCollisionResult{
            .bCollided = distance <= Radius,
            .ContactPoints = {Center}
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzCircle &other) const {
        const auto distance = glm::distance(Center, other.Center);
        const auto sumRadii = Radius + other.Radius;

        // Get the vector between circles
        const auto vecBetween = other.Center - Center;
        const auto normalizedVec = glm::normalize(vecBetween);
        const auto contactPoint = Center + normalizedVec * Radius;

        return OzzCollisionResult{
            .bCollided = distance <= sumRadii,
            .ContactPoints = {contactPoint}
        };
    }

    OzzCollisionResult OzzCircle::IsColliding(const OzzRectangle &other) const {
        return other.IsColliding(*this);
    }
}
