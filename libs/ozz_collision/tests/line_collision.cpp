//
// Created by paulm on 2026-04-02.
//

#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::shapes;

TEST(OzzLine, LinexLineCollision) {
    // Cross: horizontal vs vertical through origin
    const OzzLine horizontal{.Position = {-1.f, 0.f}, .End = {1.f, 0.f}};
    const OzzLine vertical  {.Position = { 0.f,-1.f}, .End = {0.f, 1.f}};

    // Parallel lines — no collision
    const OzzLine parallelAbove{.Position = {-1.f, 1.f}, .End = {1.f, 1.f}};

    // Lines that would intersect if extended but don't overlap as segments
    const OzzLine shortA{.Position = {0.f,  0.f}, .End = {0.5f, 0.f}};
    const OzzLine shortB{.Position = {1.f, -1.f}, .End = {1.f,  1.f}};  // passes x=1, shortA ends at x=0.5

    const auto result1 = OZZ::collision::IsColliding(horizontal, vertical);
    const auto result2 = OZZ::collision::IsColliding(horizontal, parallelAbove);
    const auto result3 = OZZ::collision::IsColliding(shortA, shortB);

    EXPECT_TRUE (result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);

    // Contact point should be at the origin
    EXPECT_NEAR(result1.ContactPoints[0].x, 0.f, 0.001f);
    EXPECT_NEAR(result1.ContactPoints[0].y, 0.f, 0.001f);
}

TEST(OzzLine, LinexLineCollision_TEndpoint) {
    // One line's endpoint touches the other line exactly (T-junction)
    const OzzLine base{.Position = {0.f, 0.f}, .End = {2.f, 0.f}};
    const OzzLine tee {.Position = {1.f, 0.f}, .End = {1.f, 1.f}};  // starts on base

    EXPECT_TRUE(OZZ::collision::IsColliding(base, tee).bCollided);
}

TEST(OzzLine, LinexRectangleCollision) {
    // Rectangle centered at origin, size 2x2 → extents x[-1..1], y[-1..1]
    const OzzRectangle rect{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const OzzLine through  {.Position = {-2.f, 0.f}, .End = {2.f,  0.f}};  // crosses left+right
    const OzzLine outside  {.Position = { 2.f, 0.f}, .End = {4.f,  0.f}};  // entirely right
    const OzzLine endInside{.Position = { 0.f, 0.f}, .End = {3.f,  0.f}};  // starts inside
    const OzzLine diagonal {.Position = {-2.f,-2.f}, .End = {0.f,  0.f}};  // ends on corner

    EXPECT_TRUE (OZZ::collision::IsColliding(rect, through).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(rect, outside).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(rect, endInside).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(rect, diagonal).bCollided);
}

TEST(OzzLine, LinexPointCollision) {
    const OzzLine diagonal{.Position = {0.f, 0.f}, .End = {2.f, 2.f}};

    const OzzPoint onLine  {.Position = {1.f, 1.f}};  // exact midpoint
    const OzzPoint offLine {.Position = {1.f, 0.f}};  // off the line
    const OzzPoint pastEnd {.Position = {3.f, 3.f}};  // collinear but beyond end

    EXPECT_TRUE (OZZ::collision::IsColliding(diagonal, onLine).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(diagonal, offLine).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(diagonal, pastEnd).bCollided);
}

TEST(OzzLine, LinexCircleCollision) {
    const OzzLine horizontal{.Position = {-2.f, 0.f}, .End = {2.f, 0.f}};

    // Circle centred on the line — hit
    const OzzCircle centred {.Position = { 0.f, 0.f  }, .Radius = 0.5f};
    // Circle above, radius reaches — hit
    const OzzCircle reaching{.Position = { 0.f, 0.4f }, .Radius = 0.5f};
    // Circle above, radius doesn't reach — miss
    const OzzCircle above   {.Position = { 0.f, 1.f  }, .Radius = 0.5f};
    // Circle beyond the end of the segment — miss
    const OzzCircle pastEnd {.Position = { 3.f, 0.f  }, .Radius = 0.5f};

    EXPECT_TRUE (OZZ::collision::IsColliding(horizontal, centred).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(horizontal, reaching).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(horizontal, above).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(horizontal, pastEnd).bCollided);
}

TEST(OzzLine, LinexPolygonCollision) {
    const OzzPolygon hex{
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    const OzzLine crossing{.Position = {0.f, 0.f  }, .End = {2.f, 0.f}};  // starts inside, exits right edge
    const OzzLine inside  {.Position = {-0.1f, 0.f}, .End = {0.1f, 0.f}}; // fully inside
    const OzzLine outside {.Position = {2.f, 0.f  }, .End = {3.f, 0.f}};  // entirely outside

    EXPECT_TRUE (OZZ::collision::IsColliding(crossing, hex).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(inside,   hex).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(outside,  hex).bCollided);
}

// ── Scale ─────────────────────────────────────────────────────────────────────

TEST(OzzLine, Scale_ExtendsLineAroundMidpoint) {
    // Horizontal line from {-1,0} to {1,0}, Scale={2,1} → extends to {-2,0}..{2,0}
    const OzzLine scaled{.Position = {-1.f, 0.f}, .End = {1.f, 0.f}, .Scale = {2.f, 1.f}};
    const OzzLine unit  {.Position = {-1.f, 0.f}, .End = {1.f, 0.f}};

    const OzzPoint inExtension{.Position = {1.5f, 0.f}};   // inside scaled, outside unit
    const OzzPoint beyondScale{.Position = {2.5f, 0.f}};   // outside both

    EXPECT_TRUE (OZZ::collision::IsColliding(scaled, inExtension).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unit,   inExtension).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(scaled, beyondScale).bCollided);
}

// ── Boundary / degenerate ────────────────────────────────────────────────────

TEST(OzzLine, ZeroLength_BehavesAsPoint) {
    // A degenerate zero-length line sits at a single point
    const OzzLine degenerate{.Position = {1.f, 0.f}, .End = {1.f, 0.f}};

    const OzzPoint atPos {.Position = {1.f, 0.f}};
    const OzzPoint nearby{.Position = {2.f, 0.f}};

    EXPECT_TRUE (OZZ::collision::IsColliding(degenerate, atPos).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(degenerate, nearby).bCollided);
}

TEST(OzzLine, PointAtEndpoint) {
    // Points exactly at start and end of a line should be on the line
    const OzzLine line{.Position = {0.f, 0.f}, .End = {3.f, 0.f}};

    const OzzPoint atStart{.Position = {0.f, 0.f}};
    const OzzPoint atEnd  {.Position = {3.f, 0.f}};

    EXPECT_TRUE(OZZ::collision::IsColliding(line, atStart).bCollided);
    EXPECT_TRUE(OZZ::collision::IsColliding(line, atEnd).bCollided);
}

TEST(OzzLine, CollinearOverlapping_ReturnsNoCollision) {
    // Two collinear overlapping segments — the cross-product method yields denom≈0
    // and returns NoCollision. This is a known limitation (not a bug), documented here.
    const OzzLine a{.Position = {-2.f, 0.f}, .End = {1.f, 0.f}};
    const OzzLine b{.Position = {-1.f, 0.f}, .End = {2.f, 0.f}};

    EXPECT_FALSE(OZZ::collision::IsColliding(a, b).bCollided);
}
