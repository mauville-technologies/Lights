//
// Created by ozzadar on 2025-07-13.
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ranges>
#include <algorithm>
#include <RtAudio.h>

#include "audio_submix.h"

namespace OZZ::lights::audio {
    struct AudioDevice {
        std::string Name;
        uint32_t ID;
        uint8_t InputChannels{0};
        uint8_t OutputChannels{0};

        [[nodiscard]] bool CanInput() const {
            return InputChannels > 0;
        }

        [[nodiscard]] bool CanOutput() const {
            return OutputChannels > 0;
        }
    };

    class AudioSubsystem {
    public:
        void Init();
        void Shutdown();

        [[nodiscard]] const std::vector<AudioDevice>& GetAudioDevices() const {
            return audioDevices;
        }

        [[nodiscard]] auto GetOutputDevices() const {
            return audioDevices | std::views::filter([](const AudioDevice& device) {
                return device.CanOutput();
            });
        }

        [[nodiscard]] auto GetCurrentInputDevice() const {
            return audioDevices | std::views::filter([](const AudioDevice& device) {
                return device.CanInput();
            });
        }

        [[nodiscard]] const AudioDevice* GetCurrentOutputDevice() const {
            return currentOutputDevice;
        }


        [[nodiscard]] bool IsInitialized() const {
            return rtAudio != nullptr;
        }

        [[nodiscard]] bool IsValidDevice(const uint32_t deviceID) const {
            return std::ranges::find_if(audioDevices, [deviceID](const AudioDevice& device) {
                return device.ID == deviceID;
            }) != audioDevices.end();
        }

        [[nodiscard]] const AudioDevice* GetAudioDevice(const uint32_t deviceID) const {
            const auto device = std::ranges::find_if(audioDevices, [deviceID](const AudioDevice& device) {
                return device.ID == deviceID;
            });
            if (device != audioDevices.end()) {
                return &(*device);
            }
            return nullptr;
        }

        void SelectOutputAudioDevice(bool bUseDefault = true, const uint32_t deviceID = 0);

    private:
        void detectAudioDevices();

        bool initializeRtAudio();
        bool initializeMainMix();

        int renderAudio(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
                        RtAudioStreamStatus status) const;

        void shutdownMainMix();
        void closeOpenStream();
        void shutdownRtAudio();

    private:
        std::unique_ptr<RtAudio> rtAudio{nullptr};
        const AudioDevice* currentOutputDevice{nullptr};
        std::vector<AudioDevice> audioDevices{};
        std::unique_ptr<AudioSubmix> mainMix{nullptr};
    };
}
