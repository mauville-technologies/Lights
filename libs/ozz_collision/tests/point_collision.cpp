//
// Created by ozzadar on 2025-01-14.
//

#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
#include "ozz_collision/shapes/ozz_rectangle.h"

using namespace OZZ::collision::shapes;

TEST(OzzPoint, PointxPointCollision) {
    constexpr OzzPoint point1{.Position = {0.f, 0.f}};
    constexpr OzzPoint point2{.Position = {0.f, 0.f}};
    constexpr OzzPoint point3{.Position = {1.f, 0.f}};

    const auto result1 = OZZ::collision::IsColliding(point1, point2);
    const auto result2 = OZZ::collision::IsColliding(point1, point3);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_EQ(result1.ContactPoints[0], point1.Position);
}

TEST(OzzPoint, PointxCircleCollision) {
    constexpr OzzPoint point1{.Position = {0.f, 0.f}};
    constexpr OzzPoint point2{.Position = {0.f, 1.f}};
    constexpr OzzCircle circle1{.Position = {0.f, 0.f}, .Radius = 0.5f};

    const auto result1 = OZZ::collision::IsColliding(point1, circle1);
    const auto result2 = OZZ::collision::IsColliding(point2, circle1);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_EQ(result1.ContactPoints[0], point1.Position);
}

TEST(OzzPoint, PointxRectangleCollision) {
    constexpr OzzPoint point1{.Position = {0.f, 0.f}};
    constexpr OzzPoint point2{.Position = {0.f, 0.5001f}};
    constexpr OzzPoint point3{.Position = {0.f, 0.5f}};

    constexpr OzzRectangle rectangle1{.Position = {0.f, 0.f}, .Size = {1.f, 1.f}};

    const auto result1 = OZZ::collision::IsColliding(point1, rectangle1);
    const auto result2 = OZZ::collision::IsColliding(point2, rectangle1);
    const auto result3 = OZZ::collision::IsColliding(point3, rectangle1);

    EXPECT_TRUE(result1.bCollided);
    EXPECT_FALSE(result2.bCollided);
    EXPECT_TRUE(result3.bCollided);
}