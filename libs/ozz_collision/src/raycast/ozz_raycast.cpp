//
// Created by Claude on 2026-04-04.
//

#include "ozz_collision/raycast/ozz_raycast.h"

#include <algorithm>
#include <cmath>

#include "ozz_collision/ozz_collision_result.h"
#include "ozz_collision/shapes/ozz_point.h"

namespace OZZ::collision::raycast {

    static constexpr float RayPlaneEpsilon = 1e-6f;

    OzzRaycastResult3D RaycastAgainstShape(
        const OzzRay3D& ray,
        const OzzPlane3D& plane,
        const shapes::OzzShapeData& shape) {

        // Step 1: Ray-plane intersection
        const float denom = glm::dot(ray.Direction, plane.Normal);
        if (std::abs(denom) < RayPlaneEpsilon) {
            return OzzRaycastResult3D::NoHit(); // ray is parallel to the plane
        }

        // Step 2: Compute parametric distance t
        const float t = glm::dot(plane.Origin - ray.Origin, plane.Normal) / denom;
        if (t < 0.f) {
            return OzzRaycastResult3D::NoHit(); // hit is behind the ray origin
        }

        // Step 3: Compute 3D hit point
        const glm::vec3 hitPoint3D = ray.Origin + t * ray.Direction;

        // Step 4: Project to local 2D coordinates on the plane
        const glm::vec3 offset = hitPoint3D - plane.Origin;
        const float localX = glm::dot(offset, plane.Right);
        const float localY = glm::dot(offset, plane.Up);
        const glm::vec2 local2D{localX, localY};

        // Step 5: Point-in-shape test using existing 2D collision
        const auto collisionResult = std::visit(
            [&](const auto& shapeData) -> OzzCollisionResult {
                const shapes::OzzPoint point{.Position = local2D};
                return IsColliding(point, shapeData);
            },
            shape);

        if (!collisionResult.bCollided) {
            return OzzRaycastResult3D::NoHit();
        }

        // Step 6: Return hit result
        return {
            .bHit = true,
            .Distance = t,
            .HitPoint3D = hitPoint3D,
            .HitPointLocal2D = local2D,
            .PlaneNormal = plane.Normal,
        };
    }

    OzzRay3D ScreenToRay(
        float ndcX, float ndcY,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix) {

        const glm::mat4 invVP = glm::inverse(projectionMatrix * viewMatrix);

        // Unproject near and far plane points
        const glm::vec4 nearClip = invVP * glm::vec4{ndcX, ndcY, -1.f, 1.f};
        const glm::vec4 farClip = invVP * glm::vec4{ndcX, ndcY, 1.f, 1.f};

        const glm::vec3 nearWorld = glm::vec3{nearClip} / nearClip.w;
        const glm::vec3 farWorld = glm::vec3{farClip} / farClip.w;

        return {
            .Origin = nearWorld,
            .Direction = glm::normalize(farWorld - nearWorld),
        };
    }

    std::optional<OzzRaycastHit3D> RaycastClosest(
        const OzzRay3D& ray,
        std::span<const std::pair<OzzPlane3D, shapes::OzzShapeData>> targets) {

        std::optional<OzzRaycastHit3D> closest;

        for (size_t i = 0; i < targets.size(); ++i) {
            const auto& [plane, shape] = targets[i];
            auto result = RaycastAgainstShape(ray, plane, shape);
            if (result.bHit) {
                if (!closest || result.Distance < closest->Result.Distance) {
                    closest = OzzRaycastHit3D{
                        .Result = result,
                        .TargetIndex = i,
                    };
                }
            }
        }

        return closest;
    }

    std::vector<OzzRaycastHit3D> RaycastAll(
        const OzzRay3D& ray,
        std::span<const std::pair<OzzPlane3D, shapes::OzzShapeData>> targets) {

        std::vector<OzzRaycastHit3D> hits;

        for (size_t i = 0; i < targets.size(); ++i) {
            const auto& [plane, shape] = targets[i];
            auto result = RaycastAgainstShape(ray, plane, shape);
            if (result.bHit) {
                hits.push_back(OzzRaycastHit3D{
                    .Result = result,
                    .TargetIndex = i,
                });
            }
        }

        std::sort(hits.begin(), hits.end(), [](const auto& a, const auto& b) {
            return a.Result.Distance < b.Result.Distance;
        });

        return hits;
    }

} // namespace OZZ::collision::raycast
