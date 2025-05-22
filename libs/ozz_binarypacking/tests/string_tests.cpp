//
// Created by paulm on 5/14/25.
//

#include <gtest/gtest.h>

#include "ozz_binary/binary.h"

// Demonstrate some basic assertions.
TEST(StringTests, ProperIdentifier) {
    using namespace OZZ::binary;

    String str {"Hello World!"};
    ASSERT_EQ(TypeIdentifier::String, static_cast<TypeIdentifier>(str));
    ASSERT_EQ(TypeIdentifier::String, static_cast<TypeIdentifier>(str.Bytes[0]));
}

TEST(StringTests, CanAssignString) {
    using namespace OZZ::binary;
    String str {};
    ASSERT_EQ(static_cast<std::string_view>(str), "");
    str = String("Ozz World!");
    ASSERT_EQ(static_cast<std::string_view>(str), "Ozz World!");
}

TEST(StringTests, SerializesToBytes) {
    using namespace OZZ::binary;
    String str {"Hello World!"};
    const std::span<uint8_t> bytes = str;
    ASSERT_EQ(bytes.size(), str.Bytes.size());
    ASSERT_EQ(bytes[0], static_cast<uint8_t>(TypeIdentifier::String));
    for (size_t i = 0; i < str.Bytes.size() - 1; ++i) {
        ASSERT_EQ(bytes[i + 1], static_cast<uint8_t>(str.Bytes[i + 1]));
    }
}