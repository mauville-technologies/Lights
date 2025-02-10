//
// Created by ozzadar on 2025-01-14.
//
#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::shapes;

TEST(OzzCircle, CirclexPointCollision) {
    constexpr OzzPoint point1{.Position = {0.f, 0.f}};
    constexpr OzzCircle circle1{.Center = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle2{.Center = {2.f, 0.f}, .Radius = 1.f};

    const auto result1 = OZZ::collision::IsColliding(point1, circle1);
    const auto result2 = OZZ::collision::IsColliding(point1, circle2);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_EQ(result1.ContactPoints[0], point1.Position);
}

TEST(OzzCircle, CirclexCircle) {
    constexpr OzzCircle circle1{.Center = {0.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle2{.Center = {1.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle3{.Center = {3.f, 0.f}, .Radius = 1.f};
    constexpr OzzCircle circle4{.Center = {2.f, 0.f}, .Radius = 1.f};

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
    const OzzCircle circle1{.Center = {2.f, 2.f}, .Radius = sqrt2}; // this one should touch at (1, 1)
    constexpr OzzCircle circle2{.Center = {2.f, 0.f}, .Radius = 1.f}; // this one should touch at (1, 0)
    // let's have a couple slightly off
    const OzzCircle circle3{.Center = {2.01f, 2.f}, .Radius = sqrt2}; // this one should no longer touch
    constexpr OzzCircle circle4{.Center = {2.01f, 0.f}, .Radius = 1.f}; // this one should no longer touch

    const auto result1 = OZZ::collision::IsColliding(rectangle1, circle1);
    const auto result2 = OZZ::collision::IsColliding(rectangle1, circle2);
    const auto result3 = OZZ::collision::IsColliding(rectangle1, circle3);
    const auto result4 = OZZ::collision::IsColliding(rectangle1, circle4);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_TRUE(result2.bCollided);
    EXPECT_FALSE(result3.bCollided);
    EXPECT_FALSE(result4.bCollided);
}