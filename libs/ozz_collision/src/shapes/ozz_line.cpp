//
// Created by ozzadar on 2025-06-24.
//

#include "ozz_collision/shapes/ozz_line.h"

#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzLine::IsColliding(const OzzLine &other) const {
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzPoint &other) const {
        // get length of the line
        const float lineLength = glm::length(End - Position);
        // get the distance from the start of the line to the point
        const float startDistance = glm::length(other.Position - Position);
        // get the distance from the end of the line to the point
        const float endDistance = glm::length(other.Position - End);

        // if the sum of the distances is less than or equal to the length of the line, then the point is on the line
        // TODO: We might want to use an adjustable tolerance here
        constexpr float tolerance = 0.001f;
        if (startDistance + endDistance <= lineLength + tolerance && startDistance + endDistance >= lineLength - tolerance) {
            // calculate the normal of the line
            // TODO: This is a simple normal, but it might not be the best for all cases
            glm::vec2 lineNormal = glm::normalize(glm::vec2(-lineVector.y, lineVector.x));

            // return the collision result
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {other.Position},
                .CollisionNormal = lineNormal
            };
        }
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzCircle &other) const {
        // are either of the endpoints of the line inside the circle?
        if (const auto endPointInside = other.IsColliding(OzzPoint{.Position = Position}); endPointInside.bCollided) {
            return endPointInside;
        }
        if (const auto startPointInside = other.IsColliding(OzzPoint{.Position = End}); startPointInside.bCollided) {
            return startPointInside;
        }

        // Get closest point on the line to the circle center
        const glm::vec2 lineVector = End - Position;
        const glm::vec2 lineToCircle = other.Position - Position;
        const float dot = glm::dot(lineToCircle, lineVector) / glm::dot(lineVector, lineVector);
        const auto closest = Position + (dot * lineVector);

        const auto bIsOnLine = IsColliding(OzzPoint{.Position = closest});
        if (!bIsOnLine.bCollided) {
            // If the closest point is not on the line, we need to check the endpoints
            return OzzCollisionResult::NoCollision();
        }

        // Now we can check if the closest point is within the circle
        return other.IsColliding(OzzPoint{.Position = closest});
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzRectangle &other) const {
    }
}
