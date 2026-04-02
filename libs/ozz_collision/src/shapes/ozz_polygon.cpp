//
// Created by ozzadar on 2025-01-14.
//

#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::shapes {
    OzzCollisionResult OzzPolygon::IsColliding(const OzzPoint& other) const {
        bool collided = false;

        // Transform vertices into world space: scale from origin, then translate
        auto worldVertex = [&](const glm::vec2& v) { return Position + v * Scale; };

        auto next = 0U;
        for (auto current = 0; current < Vertices.size(); current++) {
            next = current + 1;
            if (next == Vertices.size()) {
                next = 0;
            }

            const auto currentPoint = worldVertex(Vertices[current]);
            const auto nextPoint    = worldVertex(Vertices[next]);

            const bool yCheck = (currentPoint.y > other.Position.y) != (nextPoint.y > other.Position.y);
            const bool doesCross =
                yCheck && (other.Position.x < (nextPoint.x - currentPoint.x) * (other.Position.y - currentPoint.y) /
                                                      (nextPoint.y - currentPoint.y) +
                                                  currentPoint.x);

            if (doesCross)
                collided = !collided;
        }

        if (!collided)
            return OzzCollisionResult::NoCollision();

        // Find nearest edge — gives us normal + penetration depth
        float minDist = std::numeric_limits<float>::max();
        glm::vec2 bestNormal{};

        for (auto current = 0; current < Vertices.size(); current++) {
            next = current + 1;
            if (next == Vertices.size())
                next = 0;

            const auto currentPoint = worldVertex(Vertices[current]);
            const auto nextPoint    = worldVertex(Vertices[next]);

            const glm::vec2 edge    = nextPoint - currentPoint;
            const glm::vec2 toPoint = other.Position - currentPoint;

            float t = glm::clamp(glm::dot(toPoint, edge) / glm::dot(edge, edge), 0.f, 1.f);
            glm::vec2 closest = currentPoint + t * edge;

            float dist = glm::length(other.Position - closest);
            if (dist < minDist) {
                minDist = dist;
                // Outward normal: right-hand perpendicular of edge direction (assumes CCW winding)
                bestNormal = glm::normalize(glm::vec2(edge.y, -edge.x));
            }
        }

        return OzzCollisionResult{
            .bCollided        = true,
            .ContactPoints    = {other.Position},
            .CollisionNormal  = bestNormal,
            .PenetrationDepth = minDist,
        };
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzCircle& other) const {
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzRectangle& other) const {
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzLine& other) const {
        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzPolygon& other) const {
        return OzzCollisionResult::NoCollision();
    }
} // namespace OZZ::collision::shapes
