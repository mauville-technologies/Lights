//
// Created by ozzadar on 2025-01-14.
//
#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_line.h"
#include "ozz_collision/shapes/ozz_polygon.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::shapes;

TEST(OzzRectangle, RectanglexPointCollision) {
    constexpr OzzRectangle rectangle = {
        .Position = {1.f, 1.f},
        .Size = {2.f, 2.f}
    };

    constexpr OzzPoint point1 = {
        .Position = {1.f, 1.f}
    };

    constexpr OzzPoint point2 = {
        .Position = {2.f, 2.f}
    };

    constexpr OzzPoint point3 = {
        .Position = {2.01f, 2.01f}
    };

    const auto result1 = OZZ::collision::IsColliding(rectangle, point1);
    const auto result2 = OZZ::collision::IsColliding(rectangle, point2);
    const auto result3 = OZZ::collision::IsColliding(rectangle, point3);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);
}

TEST(OzzRectangle, RectanglexCircleCollision) {
    constexpr OzzRectangle rectangle = {
        .Position = {1.f, 1.f},
        .Size = {2.f, 2.f}
    };

    constexpr OzzCircle circle1 = {
        .Position = {1.f, 3.f},
        .Radius = 1.f
    };

    constexpr OzzCircle circle2 = {
        .Position = {3.f, 1.f},
        .Radius = 1.f
    };

    constexpr OzzCircle circle3 = {
        .Position = {3.01f, 1.f},
        .Radius = 1.f
    };

    const auto result1 = OZZ::collision::IsColliding(rectangle, circle1);
    const auto result2 = OZZ::collision::IsColliding(rectangle, circle2);
    const auto result3 = OZZ::collision::IsColliding(rectangle, circle3);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);
}

TEST(OzzRectangle, RectanglexRectangleCollision) {
    constexpr OzzRectangle rectangle1 = {
        .Position = {1.f, 1.f},
        .Size = {2.f, 2.f}
    };

    constexpr OzzRectangle rectangle2 = {
        .Position = {1.f, 2.f},
        .Size = {2.f, 2.f}
    };

    constexpr OzzRectangle rectangle3 = {
        .Position = {3.f, 1.f},
        .Size = {2.f, 2.f}
    };

    constexpr OzzRectangle rectangle4 = {
        .Position = {3.01f, 1.f},
        .Size = {2.f, 2.f}
    };

    const auto result1 = OZZ::collision::IsColliding(rectangle1, rectangle2);
    const auto result2 = OZZ::collision::IsColliding(rectangle1, rectangle3);
    const auto result3 = OZZ::collision::IsColliding(rectangle1, rectangle4);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);
}
TEST(OzzRectangle, RectanglexLineCollision) {
    // Rectangle centered at (1,1), size 2x2 → extents: x[0..2], y[0..2]
    constexpr OzzRectangle rect{.Position = {1.f, 1.f}, .Size = {2.f, 2.f}};

    // Line crossing left-right through the middle
    const OzzLine through{.Position = {-1.f, 1.f}, .End = {3.f, 1.f}};
    // Line entirely outside
    const OzzLine outside{.Position = {3.f, 0.f}, .End = {5.f, 0.f}};
    // Line with endpoint inside the rectangle
    const OzzLine endpointInside{.Position = {1.f, 1.f}, .End = {5.f, 1.f}};
    // Diagonal crossing a corner
    const OzzLine diagonal{.Position = {-1.f, -1.f}, .End = {1.f, 1.f}};
    // Parallel to top edge, just above → no collision
    const OzzLine aboveTop{.Position = {-1.f, 2.01f}, .End = {3.f, 2.01f}};

    EXPECT_TRUE (OZZ::collision::IsColliding(rect, through).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(rect, outside).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(rect, endpointInside).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(rect, diagonal).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(rect, aboveTop).bCollided);
}

TEST(OzzRectangle, RectanglexPolygonCollision) {
    // Unit hex centered at origin
    const OzzPolygon hex{
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    // Small rect fully inside the hex
    const OzzRectangle inside  {.Position = {0.f, 0.f}, .Size = {0.2f, 0.2f}};
    // Rect straddling the right edge (at x=0.866)
    const OzzRectangle straddle{.Position = {0.866f, 0.f}, .Size = {0.4f, 0.4f}};
    // Rect completely outside
    const OzzRectangle outside {.Position = {3.f, 0.f}, .Size = {0.5f, 0.5f}};

    EXPECT_TRUE (OZZ::collision::IsColliding(inside,   hex).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(straddle, hex).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(outside,  hex).bCollided);
}

// ── Scale ─────────────────────────────────────────────────────────────────────

TEST(OzzRectangle, Scale_AffectsSize) {
    // 2x2 rect with Scale={2,1} → effective size 4x2, extents x[-2..2], y[-1..1]
    const OzzRectangle scaled{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}, .Scale = {2.f, 1.f}};
    const OzzRectangle unit  {.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const OzzPoint inExtension{.Position = {1.5f, 0.f}};  // inside scaled (x∈[-2..2]), inside unit (x∈[-1..1])
    const OzzPoint farRight   {.Position = {1.5f, 0.f}};  // inside scaled but...
    // point outside the unit (x=1.5 > unit half-size=1) but inside scaled (x=1.5 < 2)
    const OzzPoint unitOutside{.Position = {1.2f, 0.f}};

    EXPECT_TRUE (OZZ::collision::IsColliding(scaled, unitOutside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unit,   unitOutside).bCollided);

    // Beyond scaled extents
    const OzzPoint beyondScale{.Position = {2.1f, 0.f}};
    EXPECT_FALSE(OZZ::collision::IsColliding(scaled, beyondScale).bCollided);
}

// ── Boundary ──────────────────────────────────────────────────────────────────

TEST(OzzRectangle, PointOnCorner) {
    // Rect centered at origin, size 2x2 → corners at (±1, ±1)
    const OzzRectangle rect{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const OzzPoint topRight  {.Position = { 1.f,  1.f}};
    const OzzPoint topLeft   {.Position = {-1.f,  1.f}};
    const OzzPoint bottomLeft{.Position = {-1.f, -1.f}};

    EXPECT_TRUE(OZZ::collision::IsColliding(rect, topRight).bCollided);
    EXPECT_TRUE(OZZ::collision::IsColliding(rect, topLeft).bCollided);
    EXPECT_TRUE(OZZ::collision::IsColliding(rect, bottomLeft).bCollided);
}

TEST(OzzRectangle, RectInsideRect) {
    // Small rect fully inside a large rect — should collide
    const OzzRectangle large{.Position = {0.f, 0.f}, .Size = {6.f, 6.f}};
    const OzzRectangle small{.Position = {0.f, 0.f}, .Size = {1.f, 1.f}};
    const OzzRectangle offset{.Position = {1.f, 1.f}, .Size = {1.f, 1.f}};  // still inside

    EXPECT_TRUE(OZZ::collision::IsColliding(large, small).bCollided);
    EXPECT_TRUE(OZZ::collision::IsColliding(large, offset).bCollided);
}

TEST(OzzRectangle, CircleFullyInsideRect) {
    // Circle entirely inside rect — closest point is the circle's own center
    const OzzRectangle rect{.Position = {0.f, 0.f}, .Size = {6.f, 6.f}};
    const OzzCircle inside{.Position = {0.f, 0.f}, .Radius = 0.5f};

    EXPECT_TRUE(OZZ::collision::IsColliding(rect, inside).bCollided);
}
