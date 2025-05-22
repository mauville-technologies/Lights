//
// Created by paulm on 5/13/25.
//

#pragma once

#include <array>
#include <span>
#include <cstdint>
#include "identifier.h"
#include <limits>

namespace OZZ::binary {
    struct Int8 {
        Int8() : Int8(0) {};
        Int8(const int8_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::Int8);
            Bytes[1] = static_cast<uint8_t>(inValue);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::Int8;
        }

        operator int8_t() {
            // give a warning if truncating
            return static_cast<int8_t>(Bytes[1]);
        }

        std::array<uint8_t, 2> Bytes;
    };

    struct Int16 {
        Int16() : Int16(0) {};
        Int16(const int16_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::Int16);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::Int16;
        }

        operator int16_t() {
            // give a warning if truncating
            return static_cast<int16_t>(Bytes[1] | (Bytes[2] << 8));
        }

        std::array<uint8_t, 3> Bytes;
    };

    struct Int32 {
        Int32() : Int32(0) {};
        Int32(int32_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::Int32);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
            Bytes[3] = static_cast<uint8_t>((inValue >> 16) & 0xFF);
            Bytes[4] = static_cast<uint8_t>((inValue >> 24) & 0xFF);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::Int32;
        }

        operator int32_t() {
            // give a warning if truncating
            return static_cast<int32_t>(Bytes[1] | (Bytes[2] << 8) | (Bytes[3] << 16) | (Bytes[4] << 24));
        }

        std::array<uint8_t, 5> Bytes;
    };

    struct Int64 {
        Int64() : Int64(0) {};
        Int64(int64_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::Int64);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
            Bytes[3] = static_cast<uint8_t>((inValue >> 16) & 0xFF);
            Bytes[4] = static_cast<uint8_t>((inValue >> 24) & 0xFF);
            Bytes[5] = static_cast<uint8_t>((inValue >> 32) & 0xFF);
            Bytes[6] = static_cast<uint8_t>((inValue >> 40) & 0xFF);
            Bytes[7] = static_cast<uint8_t>((inValue >> 48) & 0xFF);
            Bytes[8] = static_cast<uint8_t>((inValue >> 56) & 0xFF);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::Int64;
        }

        operator int64_t() {
            return static_cast<int64_t>(static_cast<int64_t>(Bytes[1])
                                        | (static_cast<int64_t>(Bytes[2]) << 8)
                                        | (static_cast<int64_t>(Bytes[3]) << 16)
                                        | (static_cast<int64_t>(Bytes[4]) << 24)
                                        | (static_cast<int64_t>(Bytes[5]) << 32)
                                        | (static_cast<int64_t>(Bytes[6]) << 40)
                                        | (static_cast<int64_t>(Bytes[7]) << 48)
                                        | (static_cast<int64_t>(Bytes[8]) << 56));
        }

        std::array<uint8_t, 9> Bytes;
    };

    struct UInt8 {
        UInt8() : UInt8(0) {};
        UInt8(uint8_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::UInt8);  // Assuming TypeIdentifier has Uint8 defined
            Bytes[1] = inValue;
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::UInt8;
        }

        operator uint8_t() {
            return static_cast<uint8_t>(Bytes[1]);
        }

        std::array<uint8_t, 2> Bytes;
    };

    struct UInt16 {
        UInt16() : UInt16(0) {};
        UInt16(uint16_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::UInt16);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::UInt16;
        }

        operator uint16_t() {
            return static_cast<uint16_t>(Bytes[1] | (Bytes[2] << 8));
        }

        std::array<uint8_t, 3> Bytes;
    };

    struct UInt32 {
        UInt32() : UInt32(0) {};
        UInt32(uint32_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::UInt32);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
            Bytes[3] = static_cast<uint8_t>((inValue >> 16) & 0xFF);
            Bytes[4] = static_cast<uint8_t>((inValue >> 24) & 0xFF);
        }

        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }

        operator const TypeIdentifier() {
            return TypeIdentifier::UInt32;
        }

        operator uint32_t() {
            return static_cast<uint32_t>(Bytes[1] | (Bytes[2] << 8) | (Bytes[3] << 16) | (Bytes[4] << 24));
        }

        std::array<uint8_t, 5> Bytes;
    };

    struct UInt64 {
        UInt64() : UInt64(0) {};
        UInt64(uint64_t inValue) {
            Bytes[0] = static_cast<uint8_t>(TypeIdentifier::UInt64);
            Bytes[1] = static_cast<uint8_t>(inValue & 0xFF);
            Bytes[2] = static_cast<uint8_t>((inValue >> 8) & 0xFF);
            Bytes[3] = static_cast<uint8_t>((inValue >> 16) & 0xFF);
            Bytes[4] = static_cast<uint8_t>((inValue >> 24) & 0xFF);
            Bytes[5] = static_cast<uint8_t>((inValue >> 32) & 0xFF);
            Bytes[6] = static_cast<uint8_t>((inValue >> 40) & 0xFF);
            Bytes[7] = static_cast<uint8_t>((inValue >> 48) & 0xFF);
            Bytes[8] = static_cast<uint8_t>((inValue >> 56) & 0xFF);
        }
        operator std::span<uint8_t>() {
            return std::span(Bytes);
        }
        operator const TypeIdentifier() {
            return TypeIdentifier::UInt64;
        }
        operator uint64_t() {
            return static_cast<uint64_t>(Bytes[1]
                | (static_cast<uint64_t>(Bytes[2]) << 8)
                | (static_cast<uint64_t>(Bytes[3]) << 16)
                | (static_cast<uint64_t>(Bytes[4] << 24))
                | (static_cast<uint64_t>(Bytes[5]) << 32)
                | (static_cast<uint64_t>(Bytes[6]) << 40)
                | (static_cast<uint64_t>(Bytes[7]) << 48)
                | (static_cast<uint64_t>(Bytes[8]) << 56));
        }

        std::array<uint8_t, 9> Bytes;
    };
}
