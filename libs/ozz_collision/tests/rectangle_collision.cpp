//
// Created by ozzadar on 2025-01-14.
//
#include <gtest/gtest.h>

#include "ozz_collision/shapes/ozz_point.h"
#include "ozz_collision/shapes/ozz_circle.h"
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
        .Center = {1.f, 3.f},
        .Radius = 1.f
    };

    constexpr OzzCircle circle2 = {
        .Center = {3.f, 1.f},
        .Radius = 1.f
    };

    constexpr OzzCircle circle3 = {
        .Center = {3.01f, 1.f},
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