//
// Created by ozzadar on 2024-12-17.
//

#include <spdlog/spdlog.h>
#include <lights/util/configuration.h>
#include <lights/algo/graphs/node.h>
#include <filesystem>

#include "lights/audio/audio_subsystem.h"

struct CommandLineArguments {
    std::string SampleConfigValueOne;
    std::string SampleConfigValueTwo;;

    // Window options
    uint32_t WindowWidth{1280};
    uint32_t WindowHeight{720};
    bool WindowFullscreen{false};

    bool fromToml(toml::basic_value<toml::type_config> data) {
        try {
            SampleConfigValueOne = data.at("SampleConfigValueOne").as_string();
            SampleConfigValueTwo = data.at("SampleConfigValueTwo").as_string();
            WindowWidth = data.at("Window Settings").at("Width").as_integer();
            WindowHeight = data.at("Window Settings").at("Height").as_integer();
        }
        catch (...) {
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

void TestConfig() {
    const std::filesystem::path configFilePath = std::filesystem::current_path() / "config.toml";
    spdlog::info("Reading config file {}", configFilePath.string());
    OZZ::Configuration<CommandLineArguments> config(configFilePath);
    spdlog::info("Configuration loaded: {}", config.ToString());
    config.Config.WindowWidth = 800;
    // config.SaveConfig();
    spdlog::info("Configuration saved: {}", config.ToString());
}

int main() {
    std::unique_ptr<OZZ::lights::audio::AudioSubsystem> audioSubsystem = std::make_unique<
        OZZ::lights::audio::AudioSubsystem>();
    audioSubsystem->Init();
    audioSubsystem->SelectOutputAudioDevice(false, 130);

    using NodeType = std::shared_ptr<OZZ::lights::algo::Node<std::string>>;
    NodeType Effects = std::make_shared<OZZ::lights::algo::Node<std::string>>();
    Effects->Data = std::make_unique<std::string>("Effects");
    NodeType Music = std::make_shared<OZZ::lights::algo::Node<std::string>>();
    Music->Data = std::make_unique<std::string>("Music");
    NodeType MainMix = std::make_shared<OZZ::lights::algo::Node<std::string>>();
    MainMix->Data = std::make_unique<std::string>("MainMix");
    NodeType Sound = std::make_shared<OZZ::lights::algo::Node<std::string>>();
    Sound->Data = std::make_unique<std::string>("Sound");

    OZZ::lights::algo::Node<std::string>::Connect(Effects, Music);
    OZZ::lights::algo::Node<std::string>::Connect(Music, MainMix);
    OZZ::lights::algo::Node<std::string>::Connect(Sound, MainMix);

    auto flattened = OZZ::lights::algo::Flatten(MainMix);
    for (const auto& node : flattened) {
        spdlog::info("Flattened Node: {}", *node->Data);
    }

    auto list = OZZ::lights::algo::Kahns(MainMix);
    //
    if (list) {
        for (const auto& node : list.value()) {
            spdlog::info("Node: {}", *node);
        }
    }

    // // wait for user input before exiting
    spdlog::info("Press Enter to exit...");
    std::cin.get();
    audioSubsystem->Shutdown();
    spdlog::info("Exiting CLI tool.");
    audioSubsystem.reset();
    spdlog::info("Audio subsystem shutdown complete.");
    spdlog::info("Goodbye!");
    spdlog::shutdown();
    return 0;
}
