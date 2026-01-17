//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_rectangle.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include <algorithm>

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzRectangle::IsColliding(const OzzPoint& other) const {
        // We use the extents because the rectangle has its position at the center
        auto [Left, Right, Up, Down] = GetExtents();

        if (other.Position.x >= Left && other.Position.x <= Right && other.Position.y >= Down &&
            other.Position.y <= Up) {
            // calculate normal
            const auto distLeft = glm::abs(other.Position.x - Left);
            const auto distRight = glm::abs(other.Position.x - Right);
            const auto distUp = glm::abs(other.Position.y - Up);
            const auto distDown = glm::abs(other.Position.y - Down);

            const auto minDist = std::min({distLeft, distRight, distUp, distDown});
            glm::vec2 normal{};
            if (minDist == distLeft) {
                normal = glm::vec2{-1.f, 0.f};
            } else if (minDist == distRight) {
                normal = glm::vec2{1.f, 0.f};
            } else if (minDist == distUp) {
                normal = glm::vec2{0.f, 1.f};
            } else {
                normal = glm::vec2{0.f, -1.f};
            }

            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {other.Position},
                .CollisionNormal = normal,
                .PenetrationDepth = minDist,
            };
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzRectangle::IsColliding(const OzzCircle& other) const {
        const auto [Left, Right, Up, Down] = GetExtents();

        glm::vec2 closestPoint = other.Position;
        closestPoint = glm::clamp(closestPoint, glm::vec2{Left, Down}, glm::vec2{Right, Up});

        const glm::vec2 vecBetween = closestPoint - other.Position;
        const float distance = glm::length(vecBetween);

        if (distance <= other.Radius) {
            glm::vec2 collisionNormal = (distance > 0.0001f) ? glm::normalize(vecBetween) // Unit vector, not scaled
                                                             : glm::vec2{0.f, 1.f};
            float penetrationDepth = other.Radius - distance;
            return {
                .bCollided = true,
                .ContactPoints = {closestPoint},
                .CollisionNormal = collisionNormal,
                .PenetrationDepth = penetrationDepth,
            };
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzRectangle::IsColliding(const OzzRectangle& other) const {
        auto [MyLeft, MyRight, MyUp, MyDown] = GetExtents();
        auto [OtherLeft, OtherRight, OtherUp, OtherDown] = other.GetExtents();

        if (MyRight >= OtherLeft && MyLeft <= OtherRight && MyDown <= OtherUp && MyUp >= OtherDown) {
            const auto overlapLeft = MyRight - OtherLeft;
            const auto overlapRight = OtherRight - MyLeft;
            const auto overlapUp = MyUp - OtherDown;
            const auto overlapDown = OtherUp - MyDown;

            const auto minOverlap = std::min({overlapLeft, overlapRight, overlapUp, overlapDown});

            glm::vec2 normal{};
            if (minOverlap == overlapLeft) {
                normal = glm::vec2{1.f, 0.f};
            } else if (minOverlap == overlapRight) {
                normal = glm::vec2{-1.f, 0.f};
            } else if (minOverlap == overlapUp) {
                normal = glm::vec2{0.f, -1.f};
            } else {
                normal = glm::vec2{0.f, 1.f};
            }

            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {},
                .CollisionNormal = normal,
                .PenetrationDepth = minOverlap,
            };
        }

        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzRectangle::IsColliding(const OzzLine& other) const {
        return other.IsColliding(*this);
    }

    OzzRectangle::Extents OzzRectangle::GetExtents() const {
        return {.Left = Position.x - Size.x / 2.f,
                .Right = Position.x + Size.x / 2.f,
                .Up = Position.y + Size.y / 2.f,
                .Down = Position.y - Size.y / 2.f};
    }
} // namespace OZZ::collision::shapes
