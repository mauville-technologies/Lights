//
// Created by Claude on 2026-04-04.
//

#include "ozz_collision/projection/ozz_shape_projection.h"

#include <cmath>

#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

namespace OZZ::collision::projection {

    glm::vec2 ProjectPoint(const raycast::OzzPlane3D& fromPlane,
                           const raycast::OzzPlane3D& toPlane,
                           const glm::vec2& localPoint) {
        // Step 1: fromPlane local 2D → world 3D
        const glm::vec3 world3D =
            fromPlane.Origin + localPoint.x * fromPlane.Right + localPoint.y * fromPlane.Up;

        // Step 2: world 3D → toPlane local 2D
        const glm::vec3 offset = world3D - toPlane.Origin;
        return {glm::dot(offset, toPlane.Right), glm::dot(offset, toPlane.Up)};
    }

    bool ArePlanesCoplanar(const raycast::OzzPlane3D& a,
                           const raycast::OzzPlane3D& b,
                           float tolerance) {
        // Normals must be parallel (pointing same or opposite direction)
        if (std::abs(std::abs(glm::dot(a.Normal, b.Normal)) - 1.f) > tolerance) {
            return false;
        }
        // Origins must lie on the same infinite plane
        if (std::abs(glm::dot(b.Origin - a.Origin, a.Normal)) > tolerance) {
            return false;
        }
        return true;
    }

    shapes::OzzShapeData ProjectShape(const raycast::OzzPlane3D& fromPlane,
                                      const raycast::OzzPlane3D& toPlane,
                                      const shapes::OzzShapeData& shape) {
        return std::visit(
            [&](const auto& s) -> shapes::OzzShapeData {
                using T = std::decay_t<decltype(s)>;

                if constexpr (std::is_same_v<T, shapes::OzzPoint>) {
                    return shapes::OzzPoint{
                        .Position = ProjectPoint(fromPlane, toPlane, s.Position),
                    };
                } else if constexpr (std::is_same_v<T, shapes::OzzCircle>) {
                    // Parallel planes: circle stays a circle
                    if (std::abs(std::abs(glm::dot(fromPlane.Normal, toPlane.Normal)) - 1.f) < 1e-4f) {
                        return shapes::OzzCircle{
                            .Position = ProjectPoint(fromPlane, toPlane, s.Position),
                            .Radius = shapes::OzzCircle::GetScaledRadius(s.Radius, s.Scale),
                        };
                    }
                    // Non-parallel: approximate as polygon
                    constexpr int Segments = 16;
                    constexpr float PI = 3.14159265359f;
                    const float scaledRadius = shapes::OzzCircle::GetScaledRadius(s.Radius, s.Scale);
                    std::vector<glm::vec2> verts;
                    verts.reserve(Segments);
                    for (int i = 0; i < Segments; ++i) {
                        const float angle = (static_cast<float>(i) / static_cast<float>(Segments)) * 2.f * PI;
                        const glm::vec2 localPt = s.Position + glm::vec2{
                            std::cos(angle) * scaledRadius,
                            std::sin(angle) * scaledRadius,
                        };
                        verts.push_back(ProjectPoint(fromPlane, toPlane, localPt));
                    }
                    return shapes::OzzPolygon{
                        .Position = {0.f, 0.f},
                        .Scale = {1.f, 1.f},
                        .Vertices = std::move(verts),
                    };
                } else if constexpr (std::is_same_v<T, shapes::OzzRectangle>) {
                    const glm::vec2 halfSize = s.GetScaledSize() * 0.5f;
                    const glm::vec2 corners[4] = {
                        s.Position + glm::vec2{-halfSize.x,  halfSize.y},
                        s.Position + glm::vec2{ halfSize.x,  halfSize.y},
                        s.Position + glm::vec2{ halfSize.x, -halfSize.y},
                        s.Position + glm::vec2{-halfSize.x, -halfSize.y},
                    };
                    std::vector<glm::vec2> verts;
                    verts.reserve(4);
                    for (const auto& corner : corners) {
                        verts.push_back(ProjectPoint(fromPlane, toPlane, corner));
                    }
                    return shapes::OzzPolygon{
                        .Position = {0.f, 0.f},
                        .Scale = {1.f, 1.f},
                        .Vertices = std::move(verts),
                    };
                } else if constexpr (std::is_same_v<T, shapes::OzzLine>) {
                    glm::vec2 scaledStart, scaledEnd;
                    shapes::OzzLine::GetScaledEndpoints(s.Position, s.End, s.Scale, scaledStart, scaledEnd);
                    return shapes::OzzLine{
                        .Position = ProjectPoint(fromPlane, toPlane, scaledStart),
                        .End = ProjectPoint(fromPlane, toPlane, scaledEnd),
                    };
                } else if constexpr (std::is_same_v<T, shapes::OzzPolygon>) {
                    std::vector<glm::vec2> verts;
                    verts.reserve(s.Vertices.size());
                    for (const auto& v : s.Vertices) {
                        const glm::vec2 absoluteLocal = s.Position + v * s.Scale;
                        verts.push_back(ProjectPoint(fromPlane, toPlane, absoluteLocal));
                    }
                    return shapes::OzzPolygon{
                        .Position = {0.f, 0.f},
                        .Scale = {1.f, 1.f},
                        .Vertices = std::move(verts),
                    };
                }
            },
            shape);
    }

    PlaneCollisionResult IsCollidingOnPlanes(
        const raycast::OzzPlane3D& planeA, const shapes::OzzShapeData& shapeA,
        const raycast::OzzPlane3D& planeB, const shapes::OzzShapeData& shapeB) {

        if (!ArePlanesCoplanar(planeA, planeB)) {
            return PlaneCollisionResult::NoCollision();
        }

        // Project shape B into plane A's local 2D frame
        const auto projectedB = ProjectShape(planeB, planeA, shapeB);

        // Test collision using existing 2D collision
        const auto result2D = std::visit(
            [](const auto& a, const auto& b) -> OzzCollisionResult {
                return IsColliding(a, b);
            },
            shapeA, projectedB);

        if (!result2D.bCollided) {
            return PlaneCollisionResult::NoCollision();
        }

        // Map 2D collision normal back to world 3D
        const glm::vec3 rawNormal =
            result2D.CollisionNormal.x * planeA.Right +
            result2D.CollisionNormal.y * planeA.Up;
        const float normalLen = glm::length(rawNormal);
        const glm::vec3 worldNormal = normalLen > 1e-6f ? rawNormal / normalLen : glm::vec3{0.f};

        return {
            .Result2D = result2D,
            .WorldCollisionNormal = worldNormal,
        };
    }

} // namespace OZZ::collision::projection
