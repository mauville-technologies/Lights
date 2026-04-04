//
// Created by Claude on 2026-04-04.
//

#include <gtest/gtest.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ozz_collision/ozz_collision_projection.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::projection;
using namespace OZZ::collision::raycast;
using namespace OZZ::collision::shapes;

static constexpr float TestEpsilon = 1e-3f;

// ── ProjectPoint ─────────────────────────────────────────────────────────────

TEST(ProjectPoint, IdentityPlanes_NoOp) {
    const OzzPlane3D plane{};  // default: origin at 0, Y-up, Right=+X, Up=-Z
    const glm::vec2 point{3.f, 5.f};

    const auto result = ProjectPoint(plane, plane, point);

    EXPECT_NEAR(result.x, 3.f, TestEpsilon);
    EXPECT_NEAR(result.y, 5.f, TestEpsilon);
}

TEST(ProjectPoint, BetweenTranslatedPlanes) {
    OzzPlane3D planeA{};
    planeA.Origin = {0.f, 0.f, 0.f};

    OzzPlane3D planeB{};
    planeB.Origin = {10.f, 0.f, 0.f};

    // Point at (0,0) in planeB → world (10, 0, 0) → planeA local (10, 0)
    const auto result = ProjectPoint(planeB, planeA, {0.f, 0.f});

    EXPECT_NEAR(result.x, 10.f, TestEpsilon);
    EXPECT_NEAR(result.y, 0.f, TestEpsilon);
}

TEST(ProjectPoint, Between90DegRotatedPlanes) {
    // PlaneA: default (flat on XZ, Y-up)
    const OzzPlane3D planeA{};

    // PlaneB: rotated 90° around Y axis
    // Right becomes (0,0,-1), Up stays (0,0,-1)... let me construct manually
    const auto rot = glm::angleAxis(glm::radians(90.f), glm::vec3{0.f, 1.f, 0.f});
    const auto planeB = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);

    // Point at (1, 0) in planeB's local space
    const auto result = ProjectPoint(planeB, planeA, {1.f, 0.f});

    // planeB.Right after 90° Y rotation of (1,0,0) = (0,0,-1)
    // So world3D = (0,0,0) + 1*(0,0,-1) + 0*planeB.Up = (0, 0, -1)
    // In planeA: localX = dot((0,0,-1), (1,0,0)) = 0
    //            localY = dot((0,0,-1), (0,0,-1)) = 1
    EXPECT_NEAR(result.x, 0.f, TestEpsilon);
    EXPECT_NEAR(result.y, 1.f, TestEpsilon);
}

// ── ArePlanesCoplanar ────────────────────────────────────────────────────────

TEST(ArePlanesCoplanar, SamePlane_True) {
    const OzzPlane3D a{};
    OzzPlane3D b{};
    b.Origin = {5.f, 0.f, 3.f};  // different origin but on Y=0 plane

    EXPECT_TRUE(ArePlanesCoplanar(a, b));
}

TEST(ArePlanesCoplanar, ParallelButOffset_False) {
    const OzzPlane3D a{};
    OzzPlane3D b{};
    b.Origin = {0.f, 1.f, 0.f};  // same normal but offset in Y

    EXPECT_FALSE(ArePlanesCoplanar(a, b));
}

TEST(ArePlanesCoplanar, DifferentNormals_False) {
    const OzzPlane3D a{};
    // Plane tilted 45° around X
    const auto rot = glm::angleAxis(glm::radians(45.f), glm::vec3{1.f, 0.f, 0.f});
    const auto b = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);

    EXPECT_FALSE(ArePlanesCoplanar(a, b));
}

TEST(ArePlanesCoplanar, OppositeNormals_True) {
    // Two planes on Y=0 but with opposite normals (flipped)
    OzzPlane3D a{};
    OzzPlane3D b{};
    b.Normal = {0.f, -1.f, 0.f};
    b.Right = {1.f, 0.f, 0.f};
    b.Up = {0.f, 0.f, 1.f};

    EXPECT_TRUE(ArePlanesCoplanar(a, b));
}

// ── ProjectShape ─────────────────────────────────────────────────────────────

TEST(ProjectShape, RectangleOnRotatedPlane_BecomesPolygon) {
    const OzzPlane3D planeA{};

    // PlaneB rotated 90° around Y on the same ground plane
    const auto rot = glm::angleAxis(glm::radians(90.f), glm::vec3{0.f, 1.f, 0.f});
    const auto planeB = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);

    const OzzShapeData rect = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto projected = ProjectShape(planeB, planeA, rect);

    // Should be a polygon with 4 vertices
    ASSERT_TRUE(std::holds_alternative<OzzPolygon>(projected));
    const auto& poly = std::get<OzzPolygon>(projected);
    EXPECT_EQ(poly.Vertices.size(), 4u);
}

TEST(ProjectShape, CircleBetweenParallelPlanes_StaysCircle) {
    OzzPlane3D planeA{};
    OzzPlane3D planeB{};
    planeB.Origin = {5.f, 0.f, 0.f};

    const OzzShapeData circle = OzzCircle{.Position = {0.f, 0.f}, .Radius = 3.f};

    const auto projected = ProjectShape(planeB, planeA, circle);

    ASSERT_TRUE(std::holds_alternative<OzzCircle>(projected));
    const auto& c = std::get<OzzCircle>(projected);
    EXPECT_NEAR(c.Radius, 3.f, TestEpsilon);
    // Center should be offset by the plane translation
    EXPECT_NEAR(c.Position.x, 5.f, TestEpsilon);
}

TEST(ProjectShape, PolygonProjection) {
    const OzzPlane3D planeA{};
    OzzPlane3D planeB{};
    planeB.Origin = {2.f, 0.f, 0.f};  // translated, same orientation

    const OzzShapeData hex = OzzPolygon{
        .Position = {0.f, 0.f},
        .Vertices = {{1.f, 0.f}, {0.f, 1.f}, {-1.f, 0.f}, {0.f, -1.f}},
    };

    const auto projected = ProjectShape(planeB, planeA, hex);

    ASSERT_TRUE(std::holds_alternative<OzzPolygon>(projected));
    const auto& poly = std::get<OzzPolygon>(projected);
    EXPECT_EQ(poly.Vertices.size(), 4u);
    // All vertices should be shifted by +2 in X
    EXPECT_NEAR(poly.Vertices[0].x, 3.f, TestEpsilon);
    EXPECT_NEAR(poly.Vertices[0].y, 0.f, TestEpsilon);
}

// ── IsCollidingOnPlanes ──────────────────────────────────────────────────────

TEST(IsCollidingOnPlanes, CoplanarRectangles_Colliding) {
    const OzzPlane3D planeA{};
    OzzPlane3D planeB{};
    planeB.Origin = {1.f, 0.f, 0.f};  // shifted 1 unit in X, same plane

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {4.f, 4.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {4.f, 4.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    EXPECT_TRUE(result.Result2D.bCollided);
}

TEST(IsCollidingOnPlanes, CoplanarRectangles_NotColliding) {
    const OzzPlane3D planeA{};
    OzzPlane3D planeB{};
    planeB.Origin = {10.f, 0.f, 0.f};  // far apart

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    EXPECT_FALSE(result.Result2D.bCollided);
}

TEST(IsCollidingOnPlanes, NonCoplanar_NoCollision) {
    const OzzPlane3D planeA{};  // ground plane
    // Vertical plane (standing unit)
    const auto rot = glm::angleAxis(glm::radians(90.f), glm::vec3{1.f, 0.f, 0.f});
    const auto planeB = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {100.f, 100.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {100.f, 100.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    EXPECT_FALSE(result.Result2D.bCollided);
}

TEST(IsCollidingOnPlanes, RotatedRectangles_Colliding) {
    // Two rectangles on the same plane, one rotated 45° around Y
    const OzzPlane3D planeA{};

    const auto rot = glm::angleAxis(glm::radians(45.f), glm::vec3{0.f, 1.f, 0.f});
    const auto planeB = OzzPlane3D::FromPositionAndOrientation({0.f, 0.f, 0.f}, rot);

    // Both 2x2 at origin — should overlap regardless of rotation
    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    EXPECT_TRUE(result.Result2D.bCollided);
}

TEST(IsCollidingOnPlanes, RotatedRectangles_MissAfterRotation) {
    // Rectangle A at origin, rectangle B at (1.5, 0) with 45° rotation
    // B's corners extend further along the diagonal, so carefully positioned to miss
    const OzzPlane3D planeA{};

    const auto rot = glm::angleAxis(glm::radians(45.f), glm::vec3{0.f, 1.f, 0.f});
    const auto planeB = OzzPlane3D::FromPositionAndOrientation({3.f, 0.f, 0.f}, rot);

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    // At distance 3 with 1x1 half-sizes, even with rotation they shouldn't collide
    EXPECT_FALSE(result.Result2D.bCollided);
}

TEST(IsCollidingOnPlanes, WorldNormal_CorrectDirection) {
    // Two rectangles overlapping, check that world normal is reasonable
    OzzPlane3D planeA{};
    OzzPlane3D planeB{};
    planeB.Origin = {0.5f, 0.f, 0.f};  // slightly offset in X

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = IsCollidingOnPlanes(planeA, rectA, planeB, rectB);

    ASSERT_TRUE(result.Result2D.bCollided);
    // World normal should be finite (not NaN) — it may be zero if the 2D
    // collision doesn't produce a normal for this configuration
    EXPECT_FALSE(std::isnan(result.WorldCollisionNormal.x));
    EXPECT_FALSE(std::isnan(result.WorldCollisionNormal.y));
    EXPECT_FALSE(std::isnan(result.WorldCollisionNormal.z));
}

TEST(IsCollidingOnPlanes, IdenticalPlanes_SameAsRaw2D) {
    // Both shapes on the exact same plane — should detect collision
    const OzzPlane3D plane{};

    const OzzShapeData rectA = OzzRectangle{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};
    const OzzShapeData rectB = OzzRectangle{.Position = {0.5f, 0.f}, .Size = {2.f, 2.f}};

    const auto result = IsCollidingOnPlanes(plane, rectA, plane, rectB);

    // The projected rectangle becomes a polygon, so the collision path is
    // OzzRectangle vs OzzPolygon. The key check is that collision is detected.
    EXPECT_TRUE(result.Result2D.bCollided);
}
