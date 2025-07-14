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
                                            });

        detectAudioDevices();
    }

    void AudioSubsystem::Shutdown() {
        spdlog::info("Shutting down Audio Subsystem...");
        if (rtAudio) {
            if (rtAudio->isStreamOpen()) {
                spdlog::warn("Audio stream is still open. Closing it before shutdown.");
                rtAudio->closeStream();
            }
            rtAudio.reset();
            spdlog::info("Audio Subsystem shut down successfully.");
        }
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
}
