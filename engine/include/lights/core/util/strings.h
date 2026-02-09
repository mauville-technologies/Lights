//
// Created by ozzadar on 2025-11-21.
//

#pragma once
#include <ranges>
#include <string>
#include <vector>

namespace OZZ::util {
    inline std::vector<std::string> tokenize(const std::string& str, char delimiter) {
        auto tokensView = str | std::views::split(delimiter) | std::views::transform([](auto&& subrange) {
                              return std::string(subrange.begin(), subrange.end());
                          });

        return {tokensView.begin(), tokensView.end()};
    }

    inline std::string toLower(const std::string& str) {
        auto lowerView = str | std::views::transform([](auto&& subrange) {
                             return static_cast<char>(std::tolower(subrange));
                         });
        return {lowerView.begin(), lowerView.end()};
    }
} // namespace OZZ::util