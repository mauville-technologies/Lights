//
// Created by paulm on 30/01/25.
//
#include <gtest/gtest.h>
#include <ozz_binarypacking.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}