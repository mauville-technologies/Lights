//
// Created by ozzadar on 2025-06-24.
//

#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

#include <array>

namespace OZZ::collision::shapes {

    OzzCollisionResult OzzLine::IsColliding(const OzzLine& other) const {
        glm::vec2 scaledStart, scaledEnd;
        GetScaledEndpoints(Position, End, Scale, scaledStart, scaledEnd);

        glm::vec2 otherStart, otherEnd;
        GetScaledEndpoints(other.Position, other.End, other.Scale, otherStart, otherEnd);

        const glm::vec2 d     = scaledEnd   - scaledStart;
        const glm::vec2 e     = otherEnd    - otherStart;
        const float     denom = d.x * e.y - d.y * e.x;

        if (std::abs(denom) < 0.0001f)
            return OzzCollisionResult::NoCollision(); // parallel / collinear

        const glm::vec2 diff = otherStart - scaledStart;
        const float     t    = (diff.x * e.y - diff.y * e.x) / denom;
        const float     u    = (diff.x * d.y - diff.y * d.x) / denom;

        if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f) {
            const glm::vec2 contact = scaledStart + t * d;
            // Normal: perpendicular of this line's direction
            const glm::vec2 normal  = glm::normalize(glm::vec2(-d.y, d.x));
            return OzzCollisionResult{
                .bCollided        = true,
                .ContactPoints    = {contact},
                .CollisionNormal  = normal,
                .PenetrationDepth = 0.f,
            };
        }

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
        glm::vec2 scaledStart, scaledEnd;
        GetScaledEndpoints(Position, End, Scale, scaledStart, scaledEnd);

        // If either endpoint is inside the rectangle, report that as the contact
        if (const auto r = other.IsColliding(OzzPoint{.Position = scaledStart}); r.bCollided)
            return r;
        if (const auto r = other.IsColliding(OzzPoint{.Position = scaledEnd}); r.bCollided)
            return r;

        // Test the line segment against each of the 4 rectangle edges using
        // parametric 2D segment intersection (cross-product method).
        auto segmentsIntersect = [](glm::vec2 p1, glm::vec2 p2,
                                    glm::vec2 p3, glm::vec2 p4,
                                    glm::vec2& outPoint) -> bool {
            const glm::vec2 d    = p2 - p1;
            const glm::vec2 e    = p4 - p3;
            const float     denom = d.x * e.y - d.y * e.x;
            if (std::abs(denom) < 0.0001f) return false; // parallel

            const glm::vec2 diff = p3 - p1;
            const float     t    = (diff.x * e.y - diff.y * e.x) / denom;
            const float     u    = (diff.x * d.y - diff.y * d.x) / denom;

            if (t >= 0.f && t <= 1.f && u >= 0.f && u <= 1.f) {
                outPoint = p1 + t * d;
                return true;
            }
            return false;
        };

        const auto [Left, Right, Up, Down] = other.GetExtents();

        struct RectEdge { glm::vec2 a, b; glm::vec2 normal; };
        const std::array<RectEdge, 4> rectEdges{{
            {{Left,  Down}, {Right, Down}, { 0.f, -1.f}},  // bottom
            {{Left,  Up  }, {Right, Up  }, { 0.f,  1.f}},  // top
            {{Left,  Down}, {Left,  Up  }, {-1.f,  0.f}},  // left
            {{Right, Down}, {Right, Up  }, { 1.f,  0.f}},  // right
        }};

        for (const auto& edge : rectEdges) {
            glm::vec2 contact;
            if (segmentsIntersect(scaledStart, scaledEnd, edge.a, edge.b, contact)) {
                return OzzCollisionResult{
                    .bCollided        = true,
                    .ContactPoints    = {contact},
                    .CollisionNormal  = edge.normal,
                    .PenetrationDepth = 0.f,
                };
            }
        }

        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzLine::IsColliding(const OzzPolygon& other) const {
        return other.IsColliding(*this);
    }
} // namespace OZZ::collision::shapes
