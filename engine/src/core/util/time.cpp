//
// Created by paulm on 2026-10-26.
//

#include <lights/core/util/time.h>

std::chrono::system_clock::time_point OZZ::lights::time::TimeFromEpoch(const int64_t epoch) {
    return std::chrono::system_clock::time_point(std::chrono::seconds(epoch));
}

int64_t OZZ::lights::time::EpochFromTime(std::chrono::system_clock::time_point time) {
    return std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
}