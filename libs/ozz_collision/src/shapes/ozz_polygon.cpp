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
        auto worldVertex = [&](const glm::vec2& v) {
            return Position + v * Scale;
        };

        auto next = 0U;
        for (auto current = 0; current < Vertices.size(); current++) {
            next = current + 1;
            if (next == Vertices.size()) {
                next = 0;
            }

            const auto currentPoint = worldVertex(Vertices[current]);
            const auto nextPoint = worldVertex(Vertices[next]);

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
            const auto nextPoint = worldVertex(Vertices[next]);

            const glm::vec2 edge = nextPoint - currentPoint;
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
            .bCollided = true,
            .ContactPoints = {other.Position},
            .CollisionNormal = bestNormal,
            .PenetrationDepth = minDist,
        };
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzCircle& other) const {
        auto worldVertex = [&](const glm::vec2& v) {
            return Position + v * Scale;
        };

        // Test every edge of the polygon as a line segment against the circle.
        // Vertices are already transformed to world space so we use Scale={1,1}.
        std::vector<glm::vec2> contactPoints;
        glm::vec2 bestNormal{};
        float maxPenetration = 0.f;
        bool anyEdgeCollides = false;

        for (auto current = 0; current < Vertices.size(); current++) {
            const auto next = (current + 1) % Vertices.size();

            const OzzLine edge{
                .Position = worldVertex(Vertices[current]),
                .End = worldVertex(Vertices[next]),
                .Scale = {1.f, 1.f},
            };
            const auto result = edge.IsColliding(other);

            if (result.bCollided) {
                anyEdgeCollides = true;
                for (const auto& cp : result.ContactPoints) {
                    contactPoints.push_back(cp);
                }
                if (result.PenetrationDepth > maxPenetration) {
                    maxPenetration = result.PenetrationDepth;
                    bestNormal = result.CollisionNormal;
                }
            }
        }

        if (anyEdgeCollides) {
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = contactPoints,
                .CollisionNormal = bestNormal,
                .PenetrationDepth = maxPenetration,
            };
        }

        // Edge tests miss the case where the circle is fully inside the polygon
        // (no edge is close enough). Fall back to a point-in-polygon check on the centre.
        const auto centerResult = IsColliding(OzzPoint{.Position = other.Position});
        if (centerResult.bCollided) {
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {other.Position},
                .CollisionNormal = centerResult.CollisionNormal,
                .PenetrationDepth = centerResult.PenetrationDepth + other.Radius,
            };
        }

        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzRectangle& other) const {
        auto worldVertex = [&](const glm::vec2& v) {
            return Position + v * Scale;
        };

        std::vector<glm::vec2> contactPoints;
        glm::vec2 bestNormal{};
        float maxPenetration = 0.f;
        bool anyEdgeCollides = false;

        for (auto current = 0; current < Vertices.size(); current++) {
            const auto next = (current + 1) % Vertices.size();

            const OzzLine edge{
                .Position = worldVertex(Vertices[current]),
                .End = worldVertex(Vertices[next]),
                .Scale = {1.f, 1.f},
            };
            const auto result = edge.IsColliding(other);

            if (result.bCollided) {
                anyEdgeCollides = true;
                for (const auto& cp : result.ContactPoints) {
                    contactPoints.push_back(cp);
                }
                if (result.PenetrationDepth > maxPenetration) {
                    maxPenetration = result.PenetrationDepth;
                    bestNormal = result.CollisionNormal;
                }
            }
        }

        if (anyEdgeCollides) {
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = contactPoints,
                .CollisionNormal = bestNormal,
                .PenetrationDepth = maxPenetration,
            };
        }

        // Handle the case where the rectangle is fully inside the polygon
        // (no edge intersects but the rect centre is inside).
        const auto centerResult = IsColliding(OzzPoint{.Position = other.Position});
        if (centerResult.bCollided) {
            return OzzCollisionResult{
                .bCollided = true,
                .ContactPoints = {other.Position},
                .CollisionNormal = centerResult.CollisionNormal,
                .PenetrationDepth = centerResult.PenetrationDepth,
            };
        }

        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzLine& other) const {
        auto worldVertex = [&](const glm::vec2& v) { return Position + v * Scale; };

        std::vector<glm::vec2> contactPoints;
        glm::vec2 bestNormal{};
        float maxPenetration = 0.f;
        bool anyEdgeCollides = false;

        for (auto current = 0; current < Vertices.size(); current++) {
            const auto next = (current + 1) % Vertices.size();

            const OzzLine edge{
                .Position = worldVertex(Vertices[current]),
                .End      = worldVertex(Vertices[next]),
                .Scale    = {1.f, 1.f},
            };
            const auto result = edge.IsColliding(other);

            if (result.bCollided) {
                anyEdgeCollides = true;
                for (const auto& cp : result.ContactPoints) {
                    contactPoints.push_back(cp);
                }
                if (result.PenetrationDepth > maxPenetration) {
                    maxPenetration = result.PenetrationDepth;
                    bestNormal     = result.CollisionNormal;
                }
            }
        }

        if (anyEdgeCollides) {
            return OzzCollisionResult{
                .bCollided        = true,
                .ContactPoints    = contactPoints,
                .CollisionNormal  = bestNormal,
                .PenetrationDepth = maxPenetration,
            };
        }

        // Handle line fully inside the polygon — check either endpoint
        glm::vec2 lineStart, lineEnd;
        OzzLine::GetScaledEndpoints(other.Position, other.End, other.Scale, lineStart, lineEnd);
        const auto endpointResult = IsColliding(OzzPoint{.Position = lineStart});
        if (endpointResult.bCollided) {
            return OzzCollisionResult{
                .bCollided        = true,
                .ContactPoints    = {lineStart},
                .CollisionNormal  = endpointResult.CollisionNormal,
                .PenetrationDepth = endpointResult.PenetrationDepth,
            };
        }

        return OzzCollisionResult::NoCollision();
    }

    OzzCollisionResult OzzPolygon::IsColliding(const OzzPolygon& other) const {
        auto worldVertex = [&](const glm::vec2& v) { return Position + v * Scale; };

        std::vector<glm::vec2> contactPoints;
        glm::vec2 bestNormal{};
        float maxPenetration = 0.f;
        bool anyEdgeCollides = false;

        // Test each edge of this polygon against the other polygon (via line<->line decomposition).
        for (auto current = 0; current < Vertices.size(); current++) {
            const auto next = (current + 1) % Vertices.size();

            const OzzLine edge{
                .Position = worldVertex(Vertices[current]),
                .End      = worldVertex(Vertices[next]),
                .Scale    = {1.f, 1.f},
            };
            const auto result = other.IsColliding(edge);

            if (result.bCollided) {
                anyEdgeCollides = true;
                for (const auto& cp : result.ContactPoints)
                    contactPoints.push_back(cp);
                if (result.PenetrationDepth > maxPenetration) {
                    maxPenetration = result.PenetrationDepth;
                    bestNormal     = result.CollisionNormal;
                }
            }
        }

        if (anyEdgeCollides) {
            return OzzCollisionResult{
                .bCollided        = true,
                .ContactPoints    = contactPoints,
                .CollisionNormal  = bestNormal,
                .PenetrationDepth = maxPenetration,
            };
        }

        // Fully-enclosed cases: one polygon sits entirely inside the other.
        // Check a vertex of other inside this, then a vertex of this inside other.
        auto otherWorldVertex = [&](const glm::vec2& v) { return other.Position + v * other.Scale; };

        if (!other.Vertices.empty()) {
            const auto result = IsColliding(OzzPoint{.Position = otherWorldVertex(other.Vertices[0])});
            if (result.bCollided) {
                return OzzCollisionResult{
                    .bCollided        = true,
                    .ContactPoints    = {otherWorldVertex(other.Vertices[0])},
                    .CollisionNormal  = result.CollisionNormal,
                    .PenetrationDepth = result.PenetrationDepth,
                };
            }
        }

        if (!Vertices.empty()) {
            const auto result = other.IsColliding(OzzPoint{.Position = worldVertex(Vertices[0])});
            if (result.bCollided) {
                return OzzCollisionResult{
                    .bCollided        = true,
                    .ContactPoints    = {worldVertex(Vertices[0])},
                    .CollisionNormal  = result.CollisionNormal,
                    .PenetrationDepth = result.PenetrationDepth,
                };
            }
        }

        return OzzCollisionResult::NoCollision();
    }
} // namespace OZZ::collision::shapes
