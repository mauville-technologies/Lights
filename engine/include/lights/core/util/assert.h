#pragma once

#include <spdlog/spdlog.h>

#include <cassert>

// Logs via spdlog::error before asserting, so the failure is visible even in a
// release build where NDEBUG strips assert() (and its condition) entirely.
#define OZZ_ASSERT(condition, message)   \
    do {                                 \
        if (!(condition)) {              \
            spdlog::error("{}", message); \
        }                                \
        assert((condition) && message);  \
    } while (0)
