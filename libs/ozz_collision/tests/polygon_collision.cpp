//
// Created by paulm on 2026-04-02.
//

#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_polygon.h"

using namespace OZZ::collision::shapes;

// Pointy-top unit hex centered at origin, CCW winding (Y-up).
// Outward normals point away from centre on each flat edge.
static const OzzPolygon unitHex{
    .Position = {0.f, 0.f},
    .Scale    = {1.f, 1.f},
    .Vertices = {
        { 0.f,     1.f  },   // top
        {-0.866f,  0.5f },   // upper-left
        {-0.866f, -0.5f },   // lower-left
        { 0.f,    -1.f  },   // bottom
        { 0.866f, -0.5f },   // lower-right
        { 0.866f,  0.5f },   // upper-right
    }};

// ── Hex: basic inside/outside ────────────────────────────────────────────────

TEST(OzzPolygon, PointInsideHex) {
    const OzzPoint center     {.Position = { 0.f,   0.f  }};
    const OzzPoint offCenter  {.Position = { 0.3f,  0.2f }};
    const OzzPoint nearBottom {.Position = { 0.f,  -0.8f }};

    const auto result1 = OZZ::collision::IsColliding(unitHex, center);
    const auto result2 = OZZ::collision::IsColliding(unitHex, offCenter);
    const auto result3 = OZZ::collision::IsColliding(unitHex, nearBottom);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_TRUE(result3.bCollided);

    EXPECT_EQ(result1.ContactPoints[0], center.Position);
    EXPECT_EQ(result2.ContactPoints[0], offCenter.Position);
    EXPECT_EQ(result3.ContactPoints[0], nearBottom.Position);
}

TEST(OzzPolygon, PointOutsideHex) {
    const OzzPoint aboveTop    {.Position = { 0.f,    1.1f }};
    const OzzPoint belowBottom {.Position = { 0.f,   -1.1f }};
    const OzzPoint rightOfHex  {.Position = { 0.9f,   0.f  }};   // right flat edge is at x=0.866
    const OzzPoint leftOfHex   {.Position = {-0.9f,   0.f  }};
    const OzzPoint pastCorner  {.Position = { 0.9f,   0.6f }};   // outside a vertex
    const OzzPoint farAway     {.Position = { 5.f,    5.f  }};

    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, aboveTop).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, belowBottom).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, rightOfHex).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, leftOfHex).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, pastCorner).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, farAway).bCollided);
}

// ── Hex: collision result fields ─────────────────────────────────────────────

TEST(OzzPolygon, HexPenetrationDepthIsPositive) {
    const OzzPoint inside{.Position = {0.f, 0.f}};
    const auto result = OZZ::collision::IsColliding(unitHex, inside);

    EXPECT_TRUE(result.bCollided);
    EXPECT_GT(result.PenetrationDepth, 0.f);
}

TEST(OzzPolygon, HexNormalPointsAwayFromNearestEdge) {
    // Near the right flat edge (at x=0.866) — outward normal should point mostly +x
    const OzzPoint nearRightEdge{.Position = {0.6f, 0.f}};
    const auto result1 = OZZ::collision::IsColliding(unitHex, nearRightEdge);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_GT(result1.CollisionNormal.x, 0.5f);

    // Near the upper-right edge — outward normal has both +x and +y components
    const OzzPoint nearUpperRight{.Position = {0.5f, 0.6f}};
    const auto result2 = OZZ::collision::IsColliding(unitHex, nearUpperRight);

    EXPECT_TRUE(result2.bCollided);
    EXPECT_GT(result2.CollisionNormal.x, 0.f);
    EXPECT_GT(result2.CollisionNormal.y, 0.f);
}

// ── Odd shapes ───────────────────────────────────────────────────────────────

TEST(OzzPolygon, Triangle) {
    // Right triangle: (0,0), (2,0), (0,2) — CCW
    const OzzPolygon triangle{
        .Position = {0.f, 0.f},
        .Scale    = {1.f, 1.f},
        .Vertices = {
            {0.f, 0.f},
            {2.f, 0.f},
            {0.f, 2.f},
        }};

    const OzzPoint inside       {.Position = {0.5f,  0.5f }};   // clearly inside
    const OzzPoint pastHypotenuse{.Position = {1.5f,  1.5f }};  // x+y > 2, outside
    const OzzPoint belowBase    {.Position = {1.f,   -0.1f }};  // below the base edge

    EXPECT_TRUE (OZZ::collision::IsColliding(triangle, inside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(triangle, pastHypotenuse).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(triangle, belowBase).bCollided);
}

TEST(OzzPolygon, ConcavePolygon_LShape) {
    // L-shape: bottom-right 2x1 bar + top-left 1x1 bar. CCW winding.
    //
    //  (0,2)─(1,2)
    //    |    |
    //  (0,1) (1,1)─(2,1)
    //    |         |
    //  (0,0)────(2,0)
    //
    const OzzPolygon lShape{
        .Position = {0.f, 0.f},
        .Scale    = {1.f, 1.f},
        .Vertices = {
            {0.f, 0.f},
            {2.f, 0.f},
            {2.f, 1.f},
            {1.f, 1.f},
            {1.f, 2.f},
            {0.f, 2.f},
        }};

    const OzzPoint inBottomBar{.Position = {1.5f, 0.5f}};   // inside the bottom bar
    const OzzPoint inLeftBar  {.Position = {0.5f, 1.5f}};   // inside the left bar
    const OzzPoint inCorner   {.Position = {0.5f, 0.5f}};   // inside the shared corner region
    const OzzPoint inCutout   {.Position = {1.5f, 1.5f}};   // inside bounding box but in the cutout
    const OzzPoint outside    {.Position = {3.f,  3.f }};   // fully outside

    EXPECT_TRUE (OZZ::collision::IsColliding(lShape, inBottomBar).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(lShape, inLeftBar).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(lShape, inCorner).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(lShape, inCutout).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(lShape, outside).bCollided);
}

TEST(OzzPolygon, ThinTriangle) {
    // Very long, thin triangle — stress test for numerical precision
    const OzzPolygon thin{
        .Position = {0.f, 0.f},
        .Scale    = {1.f, 1.f},
        .Vertices = {
            {  0.f,  0.f    },
            {100.f,  0.001f },
            {100.f, -0.001f },
        }};

    const OzzPoint nearTip    {.Position = { 99.f,  0.f    }};  // inside near the tip
    const OzzPoint nearBase   {.Position = {  1.f,  0.f    }};  // inside near the base
    const OzzPoint slightlyOff{.Position = { 50.f,  0.005f }};  // just above the upper edge

    EXPECT_TRUE (OZZ::collision::IsColliding(thin, nearTip).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(thin, nearBase).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(thin, slightlyOff).bCollided);
}

// ── Position offset ──────────────────────────────────────────────────────────

TEST(OzzPolygon, OffsetHex) {
    // Same unit hex translated to (10, 5)
    const OzzPolygon offsetHex{
        .Position = {10.f, 5.f},
        .Scale    = {1.f, 1.f},
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    const OzzPoint atNewCenter  {.Position = {10.f,  5.f  }};   // should be inside
    const OzzPoint atOldCenter  {.Position = { 0.f,  0.f  }};   // should be outside
    const OzzPoint justInside   {.Position = {10.f,  5.8f }};   // inside near top
    const OzzPoint justOutside  {.Position = {10.f,  6.1f }};   // outside above top

    EXPECT_TRUE (OZZ::collision::IsColliding(offsetHex, atNewCenter).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(offsetHex, atOldCenter).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(offsetHex, justInside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(offsetHex, justOutside).bCollided);
}

// ── Scale ─────────────────────────────────────────────────────────────────────

TEST(OzzPolygon, ScaledHex) {
    // Unit hex scaled up 2x — vertices reach out to radius 2
    const OzzPolygon scaledHex{
        .Position = {0.f, 0.f},
        .Scale    = {2.f, 2.f},
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    // Points that were outside the unit hex but inside the scaled-up version
    const OzzPoint wasOutside{.Position = {0.f, 1.5f}};    // at r=1.5, inside 2x hex
    const OzzPoint stillOut  {.Position = {0.f, 2.1f}};    // beyond the 2x top vertex

    EXPECT_TRUE (OZZ::collision::IsColliding(scaledHex, wasOutside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(scaledHex, stillOut).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unitHex, wasOutside).bCollided);   // confirm unit hex misses it
}
