//
// Created by ozzadar on 2025-06-24.
//

#include "ozz_collision/shapes/ozz_line.h"

#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"

namespace OZZ::collision::shapes {

    OzzCollisionResult OzzLine::IsColliding(const OzzLine& other) const {
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzPoint& other) const {
        glm::vec2 scaledStart, scaledEnd;
        GetScaledEndpoints(Position, End, Scale, scaledStart, scaledEnd);

        // get length of the line
        const float lineLength = glm::length(scaledEnd - scaledStart);
        // get the distance from the start of the line to the point
        const float startDistance = glm::length(other.Position - scaledStart);
        // get the distance from the end of the line to the point
        const float endDistance = glm::length(other.Position - scaledEnd);

        // if the sum of the distances is less than or equal to the length of the line, then the point is on the line
        // TODO: We might want to use an adjustable tolerance here
        constexpr float tolerance = 0.001f;
        const auto lineVector = scaledEnd - scaledStart;
        if (startDistance + endDistance <= lineLength + tolerance &&
            startDistance + endDistance >= lineLength - tolerance) {
            // calculate the normal of the line
            // TODO: This is a simple normal, but it might not be the best for all cases
            glm::vec2 lineNormal = glm::normalize(glm::vec2(-lineVector.y, lineVector.x));

            // return the collision result
            return OzzCollisionResult{
                .bCollided = true, .ContactPoints = {other.Position}, .CollisionNormal = lineNormal};
        }
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzCircle& other) const {
        glm::vec2 scaledStart, scaledEnd;
        GetScaledEndpoints(Position, End, Scale, scaledStart, scaledEnd);

        // are either of the endpoints of the line inside the circle?
        if (const auto endPointInside = other.IsColliding(OzzPoint{.Position = scaledStart});
            endPointInside.bCollided) {
            return endPointInside;
        }
        if (const auto startPointInside = other.IsColliding(OzzPoint{.Position = scaledEnd});
            startPointInside.bCollided) {
            return startPointInside;
        }

        // Get closest point on the line to the circle center
        const glm::vec2 lineVector = scaledEnd - scaledStart;
        const glm::vec2 lineToCircle = other.Position - scaledStart;
        const float dot = glm::dot(lineToCircle, lineVector) / glm::dot(lineVector, lineVector);
        const auto closest = scaledStart + (dot * lineVector);

        const auto bIsOnLine = IsColliding(OzzPoint{.Position = closest});
        if (!bIsOnLine.bCollided) {
            // If the closest point is not on the line, we need to check the endpoints
            return OzzCollisionResult::NoCollision();
        }

        // Now we can check if the closest point is within the circle
        return other.IsColliding(OzzPoint{.Position = closest});
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzRectangle& other) const {
        return OzzCollisionResult::NoCollision();
    }
} // namespace OZZ::collision::shapes
