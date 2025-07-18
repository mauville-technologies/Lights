//
// Created by ozzadar on 2025-07-13.
//

#include <lights/audio/audio_subsystem.h>

#include "spdlog/spdlog.h"

namespace OZZ::lights::audio {
    void AudioSubsystem::Init() {
        spdlog::info("Initializing Audio Subsystem...");
        initializeRtAudio();
        initializeMainMix();
    }

    void AudioSubsystem::Shutdown() {
        spdlog::info("Shutting down Audio Subsystem...");
        shutdownMainMix();
        shutdownRtAudio();
    }

    void AudioSubsystem::SelectOutputAudioDevice(const bool bUseDefault, const uint32_t deviceID) {
        const auto* device = GetAudioDevice(bUseDefault ? rtAudio->getDefaultOutputDevice() : deviceID);
        if (!device) {
            spdlog::error("Audio device with ID {} not found.", deviceID);
            return;
        }

        if (device == currentOutputDevice) {
            spdlog::info("Output device {} is already selected.", device->Name);
            return;
        }

        closeOpenStream();
        currentOutputDevice = nullptr;

        auto BufferSize = 256U; // Default buffer size
        RtAudio::StreamParameters OutParameters;
        OutParameters.deviceId = device->ID;
        OutParameters.nChannels = device->OutputChannels;
        OutParameters.firstChannel = 0;
        spdlog::info("Selected Output Device: {} (ID: {}, Channels: {})",
                     device->Name, device->ID, device->OutputChannels);

        rtAudio->openStream(
            nullptr, // No input
            &OutParameters, // Output device ID
            RTAUDIO_SINT16, // Sample format
            44100, // Sample rate
            &BufferSize, // Buffer size
            [](void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
               RtAudioStreamStatus status, void* userData) {
                const auto* audioSubsystem = static_cast<AudioSubsystem*>(userData);
                return audioSubsystem->renderAudio(outputBuffer, inputBuffer, nFrames, streamTime, status);
            },
            this
        );
        rtAudio->startStream();
        currentOutputDevice = device;
    }

    void AudioSubsystem::detectAudioDevices() {
        if (!rtAudio) {
            spdlog::error("RtAudio is not initialized. Cannot detect audio devices.");
            return;
        }
        auto devices = rtAudio->getDeviceIds();

        // scan through all devices
        for (auto id : devices) {
            auto deviceInfo = rtAudio->getDeviceInfo(id);
            spdlog::info("Device {}: {} (Input Channels: {}, Output Channels: {})",
                         id, deviceInfo.name, deviceInfo.inputChannels, deviceInfo.outputChannels);

            // store the device info
            if (deviceInfo.inputChannels > 0 || deviceInfo.outputChannels > 0) {
                AudioDevice device;
                device.ID = id;
                device.Name = deviceInfo.name;
                device.InputChannels = deviceInfo.inputChannels;
                device.OutputChannels = deviceInfo.outputChannels;
                audioDevices.push_back(device);
            }
        }
    }

    bool AudioSubsystem::initializeRtAudio() {
        rtAudio = std::make_unique<RtAudio>(RtAudio::Api::UNSPECIFIED,
                                            [this](auto ErrorType, const auto& ErrorMessage) {
                                                spdlog::error("RtAudio Error: {} - {}", static_cast<int>(ErrorType),
                                                              ErrorMessage);
                                            });

        detectAudioDevices();
        SelectOutputAudioDevice(); // Default to the first available output device
        return true;
    }

    bool AudioSubsystem::initializeMainMix() {
        return false;
    }

    int AudioSubsystem::renderAudio(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
                                    RtAudioStreamStatus status) const {
        if (status == RTAUDIO_OUTPUT_UNDERFLOW) {
            spdlog::warn("Audio output underflow detected. This may cause audio glitches.");
        }
        if (status == RTAUDIO_INPUT_OVERFLOW) {
            spdlog::warn("Audio input overflow detected. This may cause audio glitches.");
        }

        // let's do a saw wave for testing
        static float phase = 0.0f;
        const float frequency = 277.18; // C# note
        const float sampleRate = 44100.0f;
        constexpr float M_PI = 3.14159265358979323846f;
        for (unsigned int i = 0; i < nFrames; ++i) {
            float sample = 0.5f * (2.0f * (phase / (2.0f * M_PI)) - 1.0f); // Saw wave formula
            static_cast<int16_t*>(outputBuffer)[i * 2] = static_cast<int16_t>(sample * 32767); // Left channel
            static_cast<int16_t*>(outputBuffer)[i * 2 + 1] = static_cast<int16_t>(sample * 32767);
            // Right channel
            phase += (2.0f * M_PI * frequency) / sampleRate;
            if (phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;
        }
        return 0;
    }

    void AudioSubsystem::shutdownMainMix() {
        if (mainMix) {
            mainMix.reset();
        }
    }

    void AudioSubsystem::closeOpenStream() {
        if (rtAudio) {
            if (rtAudio->isStreamOpen()) {
                rtAudio->closeStream();
            }
        }
    }

    void AudioSubsystem::shutdownRtAudio() {
        audioDevices.clear();

        closeOpenStream();
        if (rtAudio) {
            rtAudio.reset();
            spdlog::info("Audio Subsystem shut down successfully.");
        }
    }
}
