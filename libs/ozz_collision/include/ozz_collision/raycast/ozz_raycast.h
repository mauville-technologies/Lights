//
// Created by Claude on 2026-04-04.
//

#pragma once

#include <optional>
#include <span>
#include <utility>
#include <vector>

#include <ozz_collision/ozz_collision_shapes.h>
#include <ozz_collision/raycast/ozz_plane3d.h>
#include <ozz_collision/raycast/ozz_ray3d.h>
#include <ozz_collision/raycast/ozz_raycast_result.h>

namespace OZZ::collision::raycast {
    // Cast a ray against a single 2D shape positioned on a 3D plane
    OzzRaycastResult3D RaycastAgainstShape(
        const OzzRay3D& ray,
        const OzzPlane3D& plane,
        const shapes::OzzShapeData& shape);

    // Construct a world-space ray from normalized device coordinates
    // ndcX, ndcY are in [-1, 1] range (center of screen = 0,0)
    OzzRay3D ScreenToRay(
        float ndcX, float ndcY,
        const glm::mat4& viewMatrix,
        const glm::mat4& projectionMatrix);

    // Find the closest hit among multiple targets
    std::optional<OzzRaycastHit3D> RaycastClosest(
        const OzzRay3D& ray,
        std::span<const std::pair<OzzPlane3D, shapes::OzzShapeData>> targets);

    // Find all hits, sorted by distance (nearest first)
    std::vector<OzzRaycastHit3D> RaycastAll(
        const OzzRay3D& ray,
        std::span<const std::pair<OzzPlane3D, shapes::OzzShapeData>> targets);
} // namespace OZZ::collision::raycast
