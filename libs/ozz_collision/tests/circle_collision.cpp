//
// Created by ozzadar on 2025-01-14.
//
#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"

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

    // calculate square root of 3/4
    const auto sqrt34 = std::sqrt(3.f/4.f);
    EXPECT_EQ(result2.ContactPoints.size(), 2);
    EXPECT_EQ(result2.ContactPoints[0], glm::vec2( 0.5f, sqrt34 ));
    EXPECT_EQ(result2.ContactPoints[1], glm::vec2( 0.5f, -sqrt34 ));

    EXPECT_EQ(result3.ContactPoints.size(), 1);
    EXPECT_EQ(result3.ContactPoints[0], glm::vec2( 1.f, 0.f ));
}