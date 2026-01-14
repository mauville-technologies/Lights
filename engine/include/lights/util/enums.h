//
// Created by ozzadar on 2025-11-28.
//

#pragma once

#include <utility>

// to_index: compact enum -> array-index helper.
// Uses std::to_underlying when available (C++23), otherwise falls back to underlying_type_t.
template <typename E>
constexpr std::size_t to_index(E e) noexcept {
    return static_cast<std::size_t>(std::to_underlying(e));
}

template <typename EnumType>
constexpr EnumType from_index(size_t i) {
    return static_cast<EnumType>(i);
}
