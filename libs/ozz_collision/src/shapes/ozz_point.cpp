//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzPoint::IsColliding(const OzzPoint &other) const {
        return OzzCollisionResult{
            .bCollided = Position == other.Position,
            .ContactPoints = {Position}
        };
    }

    OzzCollisionResult OzzPoint::IsColliding(const OzzCircle &other) const {
        return other.IsColliding(*this);
    }

    OzzCollisionResult OzzPoint::IsColliding(const OzzRectangle &other) const {
        return other.IsColliding(*this);
    }

    OzzCollisionResult OzzPoint::IsColliding(const OzzLine &other) const {
        return other.IsColliding(*this);
    }
}
