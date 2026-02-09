//
// Created by ozzadar on 2025-11-28.
//

#pragma once

#include <utility>

template <typename E>
concept IsEnum = std::is_enum_v<E>;

template <IsEnum E>
constexpr std::underlying_type_t<E> to_index(E e) noexcept {
    return std::to_underlying(e);
}

template <IsEnum E>
constexpr E from_index(std::underlying_type_t<E> i) noexcept {
    return static_cast<E>(i);
}

// Bitmask - enum operators
//
template <typename T>
concept IsBitmaskEnum = requires(T a, T b) {
    { a | b } -> std::same_as<T>;
    { a& b } -> std::same_as<T>;
    { a ^ b } -> std::same_as<T>;
    { ~a } -> std::same_as<T>;
    { a |= b } -> std::same_as<T&>;
    { a &= b } -> std::same_as<T&>;
    { a ^= b } -> std::same_as<T&>;
    std::is_enum_v<T>;
};

// opt-in bitmask enum
template <typename E>
struct enable_bitmask_operators : std::false_type {};

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E operator|(E lhs, E rhs) noexcept {
    return static_cast<E>(to_index(lhs) | to_index(rhs));
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E operator&(E lhs, E rhs) noexcept {
    return static_cast<E>(to_index(lhs) & to_index(rhs));
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E operator^(E lhs, E rhs) noexcept {
    return static_cast<E>(to_index(lhs) ^ to_index(rhs));
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E operator~(E lhs) noexcept {
    return static_cast<E>(~to_index(lhs));
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E& operator|=(E& lhs, E rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E& operator&=(E& lhs, E rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

template <IsEnum E>
    requires enable_bitmask_operators<E>::value
constexpr E& operator^=(E& lhs, E rhs) noexcept {
    lhs = lhs ^ rhs;
    return lhs;
}