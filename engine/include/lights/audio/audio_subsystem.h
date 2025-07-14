//
// Created by ozzadar on 2025-07-13.
//

#pragma once

#include <any>
#include <string>
#include <vector>
#include <memory>

#include "RtAudio.h"

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

        [[nodiscard]] bool IsInitialized() const {
            return rtAudio != nullptr;
        }

        [[nodiscard]] bool IsValidDevice(const uint32_t deviceID) const {
            return std::ranges::find_if(audioDevices, [deviceID](const AudioDevice& device) {
                return device.ID == deviceID;
            }) != audioDevices.end();
        }

        [[nodiscard]] const AudioDevice* GetAudioDevice(const uint32_t deviceID) const {
            auto device = std::ranges::find_if(audioDevices, [deviceID](const AudioDevice& device) {
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

    private:
        std::unique_ptr<RtAudio> rtAudio{nullptr};
        std::vector<AudioDevice> audioDevices;
    };
}
