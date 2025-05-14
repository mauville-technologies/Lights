//
// Created by paulm on 5/14/25.
//

#include <gtest/gtest.h>

#include "ozz_binary/types/identifier.h"
#include "ozz_binary/types/integer.h"
#include <limits>

TEST(IntegerTests, ProperIdentifier_Int8) {
    using namespace OZZ::binary;
    Int8 i8 {static_cast<int8_t>(0xF1)};

    ASSERT_EQ(TypeIdentifier::Int8, static_cast<TypeIdentifier>(i8));
    ASSERT_EQ(TypeIdentifier::Int8, static_cast<TypeIdentifier>(i8.Bytes[0]));
}

TEST(IntegerTests, CanAssignIntegers_Int8) {
    using namespace OZZ::binary;
    Int8 i8 {};

    ASSERT_EQ(static_cast<int8_t>(i8), 0);
    i8 = static_cast<int8_t>(std::numeric_limits<int8_t>::max());
    ASSERT_EQ(static_cast<int8_t>(i8), static_cast<int8_t>(std::numeric_limits<int8_t>::max()));
    i8 = static_cast<int8_t>(std::numeric_limits<int8_t>::min());
    ASSERT_EQ(static_cast<int8_t>(i8), static_cast<int8_t>(std::numeric_limits<int8_t>::min()));
}

TEST(IntegerTests, SerializesToBytes_Int8) {
    using namespace OZZ::binary;
    Int8 i8 {-1};

    const std::span<uint8_t> bytes = i8;
    ASSERT_EQ(bytes.size(), sizeof(int8_t) + 1);
    ASSERT_EQ(bytes[0], static_cast<uint8_t>(TypeIdentifier::Int8));
    ASSERT_EQ(bytes[1], 0xFF);
}

TEST(IntegerTests, ProperIdentifier_Int16) {
    using namespace OZZ::binary;
    Int16 i16 {static_cast<int16_t>(0xF1F1)};

    ASSERT_EQ(TypeIdentifier::Int16, static_cast<TypeIdentifier>(i16));
    ASSERT_EQ(TypeIdentifier::Int16, static_cast<TypeIdentifier>(i16.Bytes[0]));
}

TEST(IntegerTests, CanAssignIntegers_Int16) {
    using namespace OZZ::binary;
    Int16 i16 {};

    ASSERT_EQ(static_cast<int16_t>(i16), 0);
    i16 = static_cast<int16_t>(std::numeric_limits<int16_t>::max());
    ASSERT_EQ(static_cast<int16_t>(i16), static_cast<int16_t>(std::numeric_limits<int16_t>::max()));
    i16 = static_cast<int16_t>(std::numeric_limits<int16_t>::min());
    ASSERT_EQ(static_cast<int16_t>(i16), static_cast<int16_t>(std::numeric_limits<int16_t>::min()));
}

TEST(IntegerTests, SerializesToBytes_Int16) {
    using namespace OZZ::binary;
    Int16 i16 {-1};

    const std::span<uint8_t> bytes = i16;
    ASSERT_EQ(bytes.size(), sizeof(int16_t) + 1);
    ASSERT_EQ(bytes[0], static_cast<uint8_t>(TypeIdentifier::Int16));
    ASSERT_EQ(bytes[1], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[2], static_cast<uint8_t>(0xFF));
}

TEST(IntegerTests, ProperIdentifier_Int32) {
    using namespace OZZ::binary;
    Int32 i32 {static_cast<int32_t>(0xF1F1F1F1)};

    ASSERT_EQ(TypeIdentifier::Int32, static_cast<TypeIdentifier>(i32));
    ASSERT_EQ(TypeIdentifier::Int32, static_cast<TypeIdentifier>(i32.Bytes[0]));
}

TEST(IntegerTests, CanAssignIntegers_Int32) {
    using namespace OZZ::binary;
    Int32 i32 {};

    ASSERT_EQ(static_cast<int32_t>(i32), 0);
    i32 = static_cast<int32_t>(std::numeric_limits<int32_t>::max());
    ASSERT_EQ(static_cast<int32_t>(i32), static_cast<int32_t>(std::numeric_limits<int32_t>::max()));
    i32 = static_cast<int32_t>(std::numeric_limits<int32_t>::min());
    ASSERT_EQ(static_cast<int32_t>(i32), static_cast<int32_t>(std::numeric_limits<int32_t>::min()));
}

TEST(IntegerTests, SerializesToBytes_Int32) {
    using namespace OZZ::binary;
    Int32 i32 {-1};

    const std::span<uint8_t> bytes = i32;
    ASSERT_EQ(bytes.size(), sizeof(int32_t) + 1);
    ASSERT_EQ(bytes[0], static_cast<uint8_t>(TypeIdentifier::Int32));
    ASSERT_EQ(bytes[1], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[2], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[3], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[4], static_cast<uint8_t>(0xFF));
}

TEST(IntegerTests, ProperIdentifier_Int64) {
    using namespace OZZ::binary;
    Int64 i64 {static_cast<int64_t>(0xF1F1F1F1F1F1F1F1)};

    ASSERT_EQ(TypeIdentifier::Int64, static_cast<TypeIdentifier>(i64));
    ASSERT_EQ(TypeIdentifier::Int64, static_cast<TypeIdentifier>(i64.Bytes[0]));
}

TEST(IntegerTests, CanAssignIntegers_Int64) {
    using namespace OZZ::binary;
    Int64 i64 {};

    ASSERT_EQ(static_cast<int64_t>(i64), 0);
    i64 = static_cast<int64_t>(std::numeric_limits<int64_t>::max());
    ASSERT_EQ(static_cast<int64_t>(i64), static_cast<int64_t>(std::numeric_limits<int64_t>::max()));
    i64 = static_cast<int64_t>(std::numeric_limits<int64_t>::min());
    ASSERT_EQ(static_cast<int64_t>(i64), static_cast<int64_t>(std::numeric_limits<int64_t>::min()));
}

TEST(IntegerTests, SerializesToBytes_Int64) {
    using namespace OZZ::binary;
    Int64 i64 {-1};

    const std::span<uint8_t> bytes = i64;
    ASSERT_EQ(bytes.size(), sizeof(int64_t) + 1);
    ASSERT_EQ(bytes[0], static_cast<uint8_t>(TypeIdentifier::Int64));
    ASSERT_EQ(bytes[1], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[2], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[3], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[4], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[5], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[6], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[7], static_cast<uint8_t>(0xFF));
    ASSERT_EQ(bytes[8], static_cast<uint8_t>(0xFF));
}
