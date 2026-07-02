//
// Web (Emscripten) audio backend — stub.
//
// The browser has no RtAudio device layer. This keeps the AudioSubsystem API
// working (audio graph nodes can be created and connected via the main mix
// node) but produces no device output. A real Web Audio / SDL-audio output
// path can replace this later without touching the game code.
//
#if defined(__EMSCRIPTEN__)

#include <lights/core/audio/audio_subsystem.h>

#include "spdlog/spdlog.h"

namespace OZZ::lights::audio {

    void AudioSubsystem::Init(AudioSubsystemSettings &&inSettings) {
        settings = std::move(inSettings);
        initializeMainMix();
        bInitialized = (mainMixNode != nullptr);
        spdlog::info("Audio Subsystem (web stub) initialized — sample rate: {}, channels: {} "
                     "(no device output on web)",
                     settings.SampleRate, settings.AudioChannels);
    }

    void AudioSubsystem::Shutdown() {
        shutdownMainMix();
        bInitialized = false;
    }

    void AudioSubsystem::SelectOutputAudioDevice(uint32_t /*deviceID*/) {}

    void AudioSubsystem::detectAudioDevices() {}

    bool AudioSubsystem::initializeRtAudio() { return false; }

    bool AudioSubsystem::initializeMainMix() {
        mainMixNode = std::make_shared<AudioFanInMixerNode>();
        return true;
    }

    void AudioSubsystem::shutdownMainMix() {}

    void AudioSubsystem::closeOpenStream() {}

    void AudioSubsystem::shutdownRtAudio() {}

}

#endif // __EMSCRIPTEN__
