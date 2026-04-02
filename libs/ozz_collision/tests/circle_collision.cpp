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

TEST(OzzCircle, CirclexPointCollision) {
    constexpr OzzPoint point1{.Position = {0.f, 0.f}};
    constexpr OzzCircle circle1{.Position = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle2{.Position = {2.f, 0.f}, .Radius = 1.f};

    const auto result1 = OZZ::collision::IsColliding(point1, circle1);
    const auto result2 = OZZ::collision::IsColliding(point1, circle2);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_EQ(result1.ContactPoints[0], point1.Position);
}

TEST(OzzCircle, CirclexCircle) {
    constexpr OzzCircle circle1{.Position = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle2{.Position = {1.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle3{.Position = {3.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle4{.Position = {2.f, 0.f}, .Radius = 1.f};

    const auto result1 = OZZ::collision::IsColliding(circle1, circle2);
    const auto result2 = OZZ::collision::IsColliding(circle1, circle3);
    const auto result3 = OZZ::collision::IsColliding(circle1, circle4);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_TRUE(result3.bCollided);
}

TEST(OzzCircle, CirclexRectangle) {
    constexpr OzzRectangle rectangle1{.Position = {0.f, 0.f}, .Size = {2.f, 2.f}};

    const auto sqrt2 = std::sqrt(2.f);
    // now let's put some circles around it
    const OzzCircle circle1{.Position = {2.f, 2.f}, .Radius = sqrt2}; // this one should touch at (1, 1)
    constexpr OzzCircle circle2{.Position = {2.f, 0.f}, .Radius = 1.f}; // this one should touch at (1, 0)
    // let's have a couple slightly off
    const OzzCircle circle3{.Position = {2.01f, 2.f}, .Radius = sqrt2}; // this one should no longer touch
    constexpr OzzCircle circle4{.Position = {2.01f, 0.f}, .Radius = 1.f}; // this one should no longer touch

    const auto result1 = OZZ::collision::IsColliding(rectangle1, circle1);
    const auto result2 = OZZ::collision::IsColliding(rectangle1, circle2);
    const auto result3 = OZZ::collision::IsColliding(rectangle1, circle3);
    const auto result4 = OZZ::collision::IsColliding(rectangle1, circle4);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);
    EXPECT_FALSE(result4.bCollided);
}
TEST(OzzCircle, CirclexLineCollision) {
    // Horizontal line through origin
    const OzzLine through{.Position = {-2.f, 0.f}, .End = {2.f, 0.f}};

    // Circle centred on the line — hit
    const OzzCircle onLine {.Position = { 0.f, 0.f}, .Radius = 0.5f};
    // Circle just above, radius doesn't reach — miss
    const OzzCircle above  {.Position = { 0.f, 1.f}, .Radius = 0.5f};
    // Circle above, radius reaches — hit
    const OzzCircle reaching{.Position = { 0.f, 0.4f}, .Radius = 0.5f};

    EXPECT_TRUE (OZZ::collision::IsColliding(onLine,   through).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(above,    through).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(reaching, through).bCollided);
}

TEST(OzzCircle, CirclexPolygonCollision) {
    // Unit hex (pointy-top, radius ~1)
    const OzzPolygon hex{
        .Vertices = {
            { 0.f,     1.f  },
            {-0.866f,  0.5f },
            {-0.866f, -0.5f },
            { 0.f,    -1.f  },
            { 0.866f, -0.5f },
            { 0.866f,  0.5f },
        }};

    const OzzCircle inside {.Position = {0.f, 0.f  }, .Radius = 0.1f};  // fully inside
    const OzzCircle overlap{.Position = {0.f, 1.1f }, .Radius = 0.3f};  // overlaps top edge
    const OzzCircle outside{.Position = {0.f, 2.f  }, .Radius = 0.5f};  // far above

    EXPECT_TRUE (OZZ::collision::IsColliding(inside,  hex).bCollided);
    EXPECT_TRUE (OZZ::collision::IsColliding(overlap, hex).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(outside, hex).bCollided);
}

// ── Scale ─────────────────────────────────────────────────────────────────────

TEST(OzzCircle, Scale_AffectsRadius) {
    // Scale uses max(x,y), so {2,1} doubles the effective radius
    const OzzCircle big  {.Position = {0.f, 0.f}, .Radius = 1.f, .Scale = {2.f, 1.f}};
    const OzzCircle unit {.Position = {0.f, 0.f}, .Radius = 1.f};

    const OzzPoint justInside {.Position = {1.5f, 0.f}};  // inside 2x, outside 1x
    const OzzPoint outside    {.Position = {2.5f, 0.f}};  // outside both

    EXPECT_TRUE (OZZ::collision::IsColliding(big,  justInside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(unit, justInside).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(big,  outside).bCollided);
}

// ── Boundary / degenerate ────────────────────────────────────────────────────

TEST(OzzCircle, ZeroRadius_OnlyTouchesCenter) {
    const OzzCircle point {.Position = {0.f, 0.f}, .Radius = 0.f};

    const OzzPoint atCenter  {.Position = { 0.f,    0.f}};
    const OzzPoint slightlyOff{.Position = { 0.001f, 0.f}};

    EXPECT_TRUE (OZZ::collision::IsColliding(atCenter,   point).bCollided);
    EXPECT_FALSE(OZZ::collision::IsColliding(slightlyOff,point).bCollided);
}

TEST(OzzCircle, CircleCircle_ExactlyTangent) {
    // Two circles touching at exactly one point — should count as colliding
    constexpr OzzCircle c1{.Position = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle c2{.Position = {2.f, 0.f}, .Radius = 1.f};  // distance = sumRadii = 2

    EXPECT_TRUE(OZZ::collision::IsColliding(c1, c2).bCollided);
}

TEST(OzzCircle, CircleCircle_OneInsideOther) {
    constexpr OzzCircle big   {.Position = {0.f, 0.f}, .Radius = 3.f};
    constexpr OzzCircle small {.Position = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle offset{.Position = {1.f, 0.f}, .Radius = 1.f};  // still inside

    EXPECT_TRUE(OZZ::collision::IsColliding(big, small).bCollided);
    EXPECT_TRUE(OZZ::collision::IsColliding(big, offset).bCollided);
}

TEST(OzzCircle, CircleCircle_Concentric) {
    // Same position, same radius
    constexpr OzzCircle c{.Position = {5.f, 3.f}, .Radius = 2.f};

    EXPECT_TRUE(OZZ::collision::IsColliding(c, c).bCollided);
}
