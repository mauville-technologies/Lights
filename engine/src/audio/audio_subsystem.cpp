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


        auto mix = mainMix ? mainMix->MixAudio() : std::vector<float>(nFrames, 0.0f);

        // Ensure the output buffer is large enough
        if (outputBuffer == nullptr || nFrames == 0) {
            spdlog::error("Output buffer is null or nFrames is zero. Cannot render audio.");
            return 0;
        }
        if (mix.size() < nFrames) {
            spdlog::warn("Mix size is smaller than nFrames. Padding with zeros.");
            mix.resize(nFrames, 0.0f);
        }

        // TODO: @paulm - Handle different channel counts properly
        for (unsigned int i = 0; i < nFrames; ++i) {
            // Convert float to int16_t and write to output buffer
            static_cast<int16_t*>(outputBuffer)[i * 2] = static_cast<int16_t>(mix[i] * 32767); // Left channel
            static_cast<int16_t*>(outputBuffer)[i * 2 + 1] = static_cast<int16_t>(mix[i] * 32767); // Right channel
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
