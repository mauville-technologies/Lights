//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_rectangle.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzRectangle::IsColliding(const OzzPoint &other) const {
        // We use the extents because the rectangle has its position at the center
        auto [Left , Right, Up, Down] = GetExtents();
        return OzzCollisionResult{
            .bCollided =
                other.Position.x >= Left &&
                other.Position.x <= Right &&
                other.Position.y >= Down &&
                other.Position.y <= Up,
            .ContactPoints = {other.Position}
        };
    }

    OzzCollisionResult OzzRectangle::IsColliding(const OzzCircle &other) const {
        const auto [Left, Right, Up, Down] = GetExtents();

        glm::vec2 closestPoint = other.Position;
        closestPoint = glm::clamp(closestPoint, glm::vec2{Left, Down}, glm::vec2{Right, Up});

        const glm::vec2 vecBetween = other.Position - closestPoint;
        const float distance = glm::length(vecBetween);

        return {
            .bCollided = distance <= other.Radius,
            .ContactPoints = {closestPoint}
        };
    }

    OzzCollisionResult OzzRectangle::IsColliding(const OzzRectangle &other) const {
        auto [MyLeft, MyRight, MyUp, MyDown] = GetExtents();
        auto [OtherLeft, OtherRight, OtherUp, OtherDown] = other.GetExtents();

        if (MyRight >= OtherLeft && MyLeft <= OtherRight && MyDown <= OtherUp && MyUp >= OtherDown) {
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {}
            };
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzRectangle::Extents OzzRectangle::GetExtents() const {
        return {
            .Left = Position.x - Size.x / 2.f,
            .Right = Position.x + Size.x / 2.f,
            .Up = Position.y + Size.y / 2.f,
            .Down = Position.y - Size.y / 2.f
        };
    }
}
