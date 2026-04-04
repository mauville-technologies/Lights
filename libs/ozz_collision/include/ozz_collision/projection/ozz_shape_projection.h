//
// Created by Claude on 2026-04-04.
//

#pragma once

#include <glm/glm.hpp>

#include <ozz_collision/ozz_collision_result.h>
#include <ozz_collision/ozz_collision_shapes.h>
#include <ozz_collision/raycast/ozz_plane3d.h>

namespace OZZ::collision::projection {

    // Project a 2D point from one plane's local space to another's.
    // fromPlane local 2D → world 3D → toPlane local 2D.
    glm::vec2 ProjectPoint(const raycast::OzzPlane3D& fromPlane,
                           const raycast::OzzPlane3D& toPlane,
                           const glm::vec2& localPoint);

    // Project an entire shape from one plane's local space to another's.
    // A rotated OzzRectangle becomes an OzzPolygon (4 vertices).
    // OzzCircle stays a circle if planes are parallel, else becomes a polygon approximation.
    shapes::OzzShapeData ProjectShape(const raycast::OzzPlane3D& fromPlane,
                                      const raycast::OzzPlane3D& toPlane,
                                      const shapes::OzzShapeData& shape);

    // Check if two planes are coplanar (parallel normals + origins on the same plane).
    bool ArePlanesCoplanar(const raycast::OzzPlane3D& a,
                           const raycast::OzzPlane3D& b,
                           float tolerance = 1e-4f);

    // Collision result with world-space normal
    struct PlaneCollisionResult {
        OzzCollisionResult Result2D{};       // raw 2D result in the reference plane's local space
        glm::vec3 WorldCollisionNormal{};    // collision normal mapped back to world 3D

        static PlaneCollisionResult NoCollision() {
            return {.Result2D = OzzCollisionResult::NoCollision()};
        }
    };

    // Collide two shapes, each on their own 3D plane.
    // Non-coplanar shapes return no collision.
    PlaneCollisionResult IsCollidingOnPlanes(
        const raycast::OzzPlane3D& planeA, const shapes::OzzShapeData& shapeA,
        const raycast::OzzPlane3D& planeB, const shapes::OzzShapeData& shapeB);

} // namespace OZZ::collision::projection
