//
// Created by ozzadar on 2024-12-17.
//

#include <spdlog/spdlog.h>
#include <lights/util/configuration.h>
#include <filesystem>
struct CommandLineArguments {
    std::string SampleConfigValueOne;
    std::string SampleConfigValueTwo;;

    // Window options
    uint32_t WindowWidth {1280};
    uint32_t WindowHeight {720};
    bool WindowFullscreen {false};

    bool fromToml(toml::basic_value<toml::type_config> data) {
        try {
            SampleConfigValueOne = data.at("SampleConfigValueOne").as_string();
            SampleConfigValueTwo = data.at("SampleConfigValueTwo").as_string();
            WindowWidth = data.at("Window Settings").at("Width").as_integer();
            WindowHeight = data.at("Window Settings").at("Height").as_integer();
        } catch (...) {
            spdlog::error("Failed to parse configuration file");
            return false;
        }
        return true;
    }

    toml::basic_value<toml::type_config> toToml() const {
        toml::basic_value<toml::type_config> toml;
        toml["SampleConfigValueOne"] = SampleConfigValueOne;
        toml["SampleConfigValueTwo"] = SampleConfigValueTwo;
        toml["Window Settings"]["Width"] = WindowWidth;
        toml["Window Settings"]["Height"] = WindowHeight;
        return toml;
    }
};

int main() {
    spdlog::info("CLI TOOL PLACEHOLDER");

    const std::filesystem::path configFilePath = std::filesystem::current_path() / "config.toml";
    spdlog::info("Reading config file {}", configFilePath.string());
    OZZ::Configuration<CommandLineArguments> config (configFilePath);
    spdlog::info("Configuration loaded: {}", config.ToString());
    config.Config.WindowWidth = 800;
    // config.SaveConfig();
    spdlog::info("Configuration saved: {}", config.ToString());
    return 0;
}