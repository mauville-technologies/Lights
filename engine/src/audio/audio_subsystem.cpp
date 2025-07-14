//
// Created by ozzadar on 2025-07-13.
//

#include <lights/audio/audio_subsystem.h>

#include "spdlog/spdlog.h"

namespace OZZ::lights::audio {
    void AudioSubsystem::Init() {
        spdlog::info("Initializing Audio Subsystem...");
        rtAudio = std::make_unique<RtAudio>(RtAudio::Api::UNSPECIFIED,
                                            [this](auto ErrorType, const auto& ErrorMessage) {
                                                spdlog::error("RtAudio Error: {} - {}", static_cast<int>(ErrorType),
                                                              ErrorMessage);
                                                Shutdown();
                                            });
    }

    void AudioSubsystem::Shutdown() {
        spdlog::info("Shutting down Audio Subsystem...");
        if (rtAudio) {
            rtAudio->closeStream();
            rtAudio.reset();
            spdlog::info("Audio Subsystem shut down successfully.");
        }
    }

    void AudioSubsystem::detectAudioDevices() {}
}
