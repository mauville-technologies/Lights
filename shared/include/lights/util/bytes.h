//
// Created by ozzadar on 2024-12-17.
// Utility functions for reading values out of byte arrays
//

#pragma once

#include <cstdint>
#include <string>
#include <type_traits>

namespace OZZ::util::bytes {
    template <typename T>
    concept NotString = !std::is_same_v<T, std::string>;

    template <typename T>
    concept IsString = std::is_same_v<T, std::string>;

    template <NotString T>
    T ReadValue(const uint8_t* bytes, size_t offset) {
        T value = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            value |= bytes[offset + i] << (i * 8);
        }
        offset += sizeof(T);
        return value;
    }

    // String Version
    template <IsString T>
    T ReadValue(const uint8_t* bytes, size_t offset, size_t length) {
        size_t startingIndex = offset;
        size_t endingIndex = startingIndex + length;
        return T(bytes + startingIndex, bytes + endingIndex);
    }

}

