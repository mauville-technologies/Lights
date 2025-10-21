//
// Created by ozzadar on 2024-12-17.
//

#include "graph_exercise.h"

#include <filesystem>
#include <lights/util/configuration.h>
#include <spdlog/spdlog.h>

#include "lights/audio/audio_subsystem.h"
#include "lights/audio/nodes/audio_cue.h"
#include "lights/audio/nodes/saw_tooth_node.h"

#include "../engine/include/lights/algo/graph_node.h"

struct CommandLineArguments {
    std::string SampleConfigValueOne;
    std::string SampleConfigValueTwo;
    ;

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

void TestConfig() {
    const std::filesystem::path configFilePath = std::filesystem::current_path() / "config.toml";
    spdlog::info("Reading config file {}", configFilePath.string());
    OZZ::Configuration<CommandLineArguments> config(configFilePath);
    spdlog::info("Configuration loaded: {}", config.ToString());
    config.Config.WindowWidth = 800;
    // config.SaveConfig();
    spdlog::info("Configuration saved: {}", config.ToString());
}

constexpr OZZ::lights::audio::Note Notes[] = {
    OZZ::lights::audio::Note::C,
    OZZ::lights::audio::Note::D,
    OZZ::lights::audio::Note::E,
    OZZ::lights::audio::Note::D,
    OZZ::lights::audio::Note::E,
    OZZ::lights::audio::Note::F,
    OZZ::lights::audio::Note::E,
    OZZ::lights::audio::Note::D,
    OZZ::lights::audio::Note::C,
    OZZ::lights::audio::Note::D,
    OZZ::lights::audio::Note::E,
    OZZ::lights::audio::Note::D,
    OZZ::lights::audio::Note::C,
    OZZ::lights::audio::Note::E,
    OZZ::lights::audio::Note::D,
};

void TestAudio() {
    auto audioSubsystem = std::make_unique<OZZ::lights::audio::AudioSubsystem>();

    audioSubsystem->Init({.SampleRate = 44100, .AudioChannels = 2});

    audioSubsystem->SelectOutputAudioDevice(audioSubsystem->GetDefaultOutputDeviceID());

    // Create a new sawtooth node
    // connect to main mix

    const auto Saw = audioSubsystem->CreateAudioNode<OZZ::lights::audio::SawToothNode>();
    const auto testAudio = audioSubsystem->CreateAudioNode<OZZ::lights::audio::AudioCue>();
    testAudio->Load(std::filesystem::current_path() / "soul.wav");
    testAudio->PlayState = OZZ::lights::audio::AudioCuePlayState::Playing;
    testAudio->LoopMode = OZZ::lights::audio::AudioCueLoopMode::Loop;
    audioSubsystem->ConnectToMainMixNode(testAudio.get());
    // audioSubsystem->ConnectToMainMixNode(Saw.get());

    static auto timeElapsed = 0.f;
    static auto lastTickTime = std::chrono::high_resolution_clock::now();
    static constexpr float changeInterval = 0.5f; // Change note every 5 seconds
    static auto currentNoteIndex = 0;
    Saw->SetNote(Notes[currentNoteIndex], OZZ::lights::audio::Octave::Oct4);

    // every 5 seconds, change the note on the sawtooth node
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTickTime).count();
        lastTickTime = currentTime;
        timeElapsed += deltaTime;

        if (timeElapsed >= changeInterval) {
            currentNoteIndex = (currentNoteIndex + 1) % std::size(Notes);
            // Change the note on the sawtooth node
            Saw->SetNote(Notes[currentNoteIndex], OZZ::lights::audio::Octave::Oct4);
            timeElapsed = 0.f;
        }
    }
    spdlog::info("Press Enter to exit...");
    std::cin.get();
    audioSubsystem->Shutdown();
    audioSubsystem.reset();
}

int main() {
    // TestAudio();
    auto exercise = Game();
    exercise.Run();
    spdlog::info("Exiting CLI tool.");
    spdlog::info("Audio subsystem shutdown complete.");
    spdlog::info("Goodbye!");
    spdlog::shutdown();
    return 0;
}
