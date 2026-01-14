//
// Created by ozzadar on 2025-12-28.
//

#pragma once
#include <cstddef>

constexpr std::size_t operator"" _KiB(unsigned long long x) {
    return x * 1024ULL;
}

constexpr std::size_t operator"" _MiB(unsigned long long x) {
    return x * 1024ULL * 1024ULL;
}

constexpr std::size_t operator"" _GiB(unsigned long long x) {
    return x * 1024ULL * 1024ULL * 1024ULL;
}