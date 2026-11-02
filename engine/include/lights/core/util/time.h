//
// Created by paulm on 2026-10-26.
//

#pragma once
#include <chrono>

namespace OZZ::lights::time {
    std::chrono::system_clock::time_point TimeFromEpoch(int64_t epoch);
    int64_t EpochFromTime(std::chrono::system_clock::time_point time);
} // namespace OZZ::lights::time