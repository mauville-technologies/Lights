//
// Created by Claude on 2026-04-04.
//

#include <gtest/gtest.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ozz_collision/ozz_collision_raycast.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::raycast;
using namespace OZZ::collision::shapes;

static constexpr float TestEpsilon = 1e-4f;

// ── OzzPlane3D Factories ─────────────────────────────────────────────────────

TEST(OzzPlane3D, FromIdentityQuaternion) {
    const auto plane = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, glm::quat{1.f, 0.f, 0.f, 0.f});

    // Default: flat on XZ plane, Y-up
    EXPECT_NEAR(plane.Normal.y, 1.f, TestEpsilon);
    EXPECT_NEAR(plane.Right.x, 1.f, TestEpsilon);
    EXPECT_NEAR(plane.Up.z, -1.f, TestEpsilon);
}

TEST(OzzPlane3D, FromPositionAndOrientation_90DegRotationAroundX) {
    // Rotate 90 degrees around X axis: XZ plane becomes XY plane
    const auto rot = glm::angleAxis(glm::radians(90.f), glm::vec3{1.f, 0.f, 0.f});
    const auto plane = OzzPlane3D::FromPositionAndOrientation({5.f, 0.f, 0.f}, rot);

    EXPECT_NEAR(plane.Origin.x, 5.f, TestEpsilon);
    // Normal should now point along +Z (rotated from +Y)
    EXPECT_NEAR(plane.Normal.z, 1.f, TestEpsilon);
    EXPECT_NEAR(std::abs(plane.Normal.x), 0.f, TestEpsilon);
    EXPECT_NEAR(std::abs(plane.Normal.y), 0.f, TestEpsilon);
}

TEST(OzzPlane3D, FromModelMatrix_Identity) {
    const glm::mat4 identity{1.f};
    const auto plane = OzzPlane3D::FromModelMatrix(identity);

    EXPECT_NEAR(plane.Origin.x, 0.f, TestEpsilon);
    EXPECT_NEAR(plane.Origin.y, 0.f, TestEpsilon);
    EXPECT_NEAR(plane.Origin.z, 0.f, TestEpsilon);
    EXPECT_NEAR(plane.Right.x, 1.f, TestEpsilon);
    EXPECT_NEAR(plane.Up.y, 1.f, TestEpsilon);
}

TEST(OzzPlane3D, FromModelMatrix_WithTranslation) {
    auto model = glm::translate(glm::mat4{1.f}, glm::vec3{3.f, 5.f, 7.f});
    const auto plane = OzzPlane3D::FromModelMatrix(model);

    EXPECT_NEAR(plane.Origin.x, 3.f, TestEpsilon);
    EXPECT_NEAR(plane.Origin.y, 5.f, TestEpsilon);
    EXPECT_NEAR(plane.Origin.z, 7.f, TestEpsilon);
}

// ── Ray-Plane Basics ─────────────────────────────────────────────────────────

TEST(Raycast, RayHitsPlaneAtKnownDistance) {
    // Ray pointing straight down at a flat XZ plane at Y=0
    const OzzRay3D ray{.Origin = {0.f, 10.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};  // default: origin at 0, normal Y-up
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {100.f, 100.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    EXPECT_NEAR(result.Distance, 10.f, TestEpsilon);
    EXPECT_NEAR(result.HitPoint3D.y, 0.f, TestEpsilon);
}

TEST(Raycast, RayParallelToPlane_NoHit) {
    // Ray parallel to the XZ plane (moving along X)
    const OzzRay3D ray{.Origin = {0.f, 5.f, 0.f}, .Direction = {1.f, 0.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {100.f, 100.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_FALSE(result.bHit);
}

TEST(Raycast, RayPointingAway_NoHit) {
    // Ray pointing up, away from the plane at Y=0
    const OzzRay3D ray{.Origin = {0.f, 5.f, 0.f}, .Direction = {0.f, 1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {100.f, 100.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_FALSE(result.bHit);
}

// ── Raycast vs Rectangle ─────────────────────────────────────────────────────

TEST(Raycast, HitsRectangleCenter) {
    // Rectangle at origin on XZ plane, 2x2
    const OzzRay3D ray{.Origin = {0.f, 5.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    EXPECT_NEAR(result.HitPointLocal2D.x, 0.f, TestEpsilon);
    EXPECT_NEAR(result.HitPointLocal2D.y, 0.f, TestEpsilon);
}

TEST(Raycast, MissesRectangle_HitsPlaneButOutsideShape) {
    // Ray hits the plane but outside the rectangle bounds
    const OzzRay3D ray{.Origin = {5.f, 5.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_FALSE(result.bHit);
}

// ── Raycast vs Circle ────────────────────────────────────────────────────────

TEST(Raycast, HitsCircleOnFlatPlane) {
    const OzzRay3D ray{.Origin = {0.f, 10.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzCircle{.Position = {0.f, 0.f}, .Radius = 5.f};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    EXPECT_NEAR(result.Distance, 10.f, TestEpsilon);
}

TEST(Raycast, MissesCircle) {
    // Ray hits plane at local (10, 0), circle is at origin with radius 5
    const OzzRay3D ray{.Origin = {10.f, 10.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData shape = OzzCircle{.Position = {0.f, 0.f}, .Radius = 5.f};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_FALSE(result.bHit);
}

TEST(Raycast, HitsCircleOnTiltedPlane) {
    // Circle on a plane tilted 45 degrees around X axis
    const auto rot = glm::angleAxis(glm::radians(45.f), glm::vec3{1.f, 0.f, 0.f});
    const auto plane = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);
    const OzzShapeData shape = OzzCircle{.Position = {0.f, 0.f}, .Radius = 5.f};

    // Ray going straight down should hit the tilted plane
    const OzzRay3D ray{.Origin = {0.f, 10.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    EXPECT_NEAR(result.HitPoint3D.x, 0.f, TestEpsilon);
}

// ── Raycast vs Polygon (Hexagon) ────────────────────────────────────────────

TEST(Raycast, HitsHexagonCenter) {
    // Unit hexagon on XZ plane
    const OzzShapeData shape = OzzPolygon{
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    const OzzRay3D ray{.Origin = {0.f, 5.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    EXPECT_NEAR(result.HitPointLocal2D.x, 0.f, TestEpsilon);
    EXPECT_NEAR(result.HitPointLocal2D.y, 0.f, TestEpsilon);
}

TEST(Raycast, MissesHexagon) {
    const OzzShapeData shape = OzzPolygon{
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    // Ray hits plane at (3, 0) in local space — well outside the hex
    const OzzRay3D ray{.Origin = {3.f, 5.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_FALSE(result.bHit);
}

// ── Local 2D Projection ─────────────────────────────────────────────────────

TEST(Raycast, Local2DProjection_OffCenter) {
    // Hit a large rectangle at a known offset
    const OzzRay3D ray{.Origin = {2.f, 10.f, -3.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};  // flat XZ, Right=+X, Up=-Z
    const OzzShapeData shape = OzzRectangle{.Position = {0.f, 0.f}, .Size = {20.f, 20.f}};

    const auto result = RaycastAgainstShape(ray, plane, shape);

    EXPECT_TRUE(result.bHit);
    // local X = dot(offset, Right) = dot((2,0,-3), (1,0,0)) = 2
    EXPECT_NEAR(result.HitPointLocal2D.x, 2.f, TestEpsilon);
    // local Y = dot(offset, Up) = dot((2,0,-3), (0,0,-1)) = 3
    EXPECT_NEAR(result.HitPointLocal2D.y, 3.f, TestEpsilon);
}

// ── ScreenToRay ──────────────────────────────────────────────────────────────

TEST(ScreenToRay, CenterOfScreen) {
    // Camera at (0,10,0) looking straight down
    const auto view = glm::lookAt(
        glm::vec3{0.f, 10.f, 0.f},
        glm::vec3{0.f, 0.f, 0.f},
        glm::vec3{0.f, 0.f, -1.f});
    const auto proj = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 100.f);

    const auto ray = ScreenToRay(0.f, 0.f, view, proj);

    // Center of screen should produce a ray pointing roughly straight down
    EXPECT_NEAR(ray.Direction.x, 0.f, TestEpsilon);
    EXPECT_NEAR(ray.Direction.y, -1.f, TestEpsilon);
    EXPECT_NEAR(ray.Direction.z, 0.f, TestEpsilon);
}

// ── Batch Queries ────────────────────────────────────────────────────────────

TEST(RaycastBatch, ClosestHit) {
    const OzzRay3D ray{.Origin = {0.f, 20.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};

    // Two rectangles at different heights
    OzzPlane3D planeHigh{};
    planeHigh.Origin.y = 10.f;
    OzzPlane3D planeLow{};
    planeLow.Origin.y = 5.f;

    const OzzShapeData rect = OzzRectangle{.Position = {0.f, 0.f}, .Size = {10.f, 10.f}};

    std::vector<std::pair<OzzPlane3D, OzzShapeData>> targets = {
        {planeLow, rect},
        {planeHigh, rect},
    };

    const auto closest = RaycastClosest(ray, targets);

    ASSERT_TRUE(closest.has_value());
    EXPECT_EQ(closest->TargetIndex, 1u);  // planeHigh is closer (at y=10)
    EXPECT_NEAR(closest->Result.Distance, 10.f, TestEpsilon);
}

TEST(RaycastBatch, AllHitsSorted) {
    const OzzRay3D ray{.Origin = {0.f, 20.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};

    OzzPlane3D plane1{};
    plane1.Origin.y = 5.f;
    OzzPlane3D plane2{};
    plane2.Origin.y = 15.f;
    OzzPlane3D plane3{};
    plane3.Origin.y = 10.f;

    const OzzShapeData rect = OzzRectangle{.Position = {0.f, 0.f}, .Size = {10.f, 10.f}};

    std::vector<std::pair<OzzPlane3D, OzzShapeData>> targets = {
        {plane1, rect},
        {plane2, rect},
        {plane3, rect},
    };

    const auto hits = RaycastAll(ray, targets);

    ASSERT_EQ(hits.size(), 3u);
    // Sorted by distance: plane2 (dist 5), plane3 (dist 10), plane1 (dist 15)
    EXPECT_EQ(hits[0].TargetIndex, 1u);
    EXPECT_EQ(hits[1].TargetIndex, 2u);
    EXPECT_EQ(hits[2].TargetIndex, 0u);
}

TEST(RaycastBatch, NoHits) {
    const OzzRay3D ray{.Origin = {100.f, 20.f, 0.f}, .Direction = {0.f, -1.f, 0.f}};
    const OzzPlane3D plane{};
    const OzzShapeData rect = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    std::vector<std::pair<OzzPlane3D, OzzShapeData>> targets = {{plane, rect}};

    const auto closest = RaycastClosest(ray, targets);
    const auto all = RaycastAll(ray, targets);

    EXPECT_FALSE(closest.has_value());
    EXPECT_TRUE(all.empty());
}
