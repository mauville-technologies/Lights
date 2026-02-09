
#pragma once

#include <utility>

//
// Created by ozzadar on 2025-05-08.
//
#include <spdlog/spdlog.h>
#include <toml.hpp>

namespace OZZ {

    // fromToml function required
    template <typename T>
    concept FromToml = requires(T t, toml::basic_value<toml::type_config> v) {
        { t.fromToml(v) } -> std::same_as<bool>;
    };

    // toToml function required
    template <typename T>
    concept ToToml = requires(T t) {
        { t.toToml() } -> std::same_as<toml::basic_value<toml::type_config>>;
    };

    /**
     * Automates the loading and saving of TOML configuration files.
     * @tparam T The type into which the configuration will be loaded
     */
    template <typename T>
        requires FromToml<T> && ToToml<T>
    struct Configuration {
        explicit Configuration(std::filesystem::path inConfigPath, const bool bSaveIfNew = true)
            : ConfigPath(std::move(inConfigPath)) {
            if (std::filesystem::exists(ConfigPath)) {
                if (!Config.fromToml(toml::parse(ConfigPath))) {
                    spdlog::warn("Failed to load configuration from file");
                }
                // We're going to put a save here -- this will make sure that the config is upgraded to the latest
                // version (if missing fields)
                SaveConfig();
            } else if (bSaveIfNew) {
                SaveConfig();
            }
        };

        void SaveConfig() {
            auto data = Config.toToml();
            std::ofstream file(ConfigPath);
            if (file.is_open()) {
                file << data;
                file.close();
            } else {
                spdlog::error("Failed to save configuration to file");
            }
        }

        std::string ToString() {
            auto data = Config.toToml();
            std::ostringstream oss;
            oss << data;
            return oss.str();
        }

        // Initialize to default
        T Config{};
        std::filesystem::path ConfigPath;
    };
} // namespace OZZ
