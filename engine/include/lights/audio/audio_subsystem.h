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

#include "audio_graph.h"
#include "nodes/audio_fan_in_mixer_node.h"

namespace OZZ::lights::audio {
    struct AudioSubsystemSettings {
        uint32_t SampleRate{44100};
        uint8_t AudioChannels{2};
    };

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
        void Init(AudioSubsystemSettings&& inSettings = {});
        void Shutdown();

        template <typename T>
        void ConnectToMainMixNode(AudioGraphNodePtr<T> node) {
            AudioGraphNodeType<T>::Connect(node, mainMixNode);
        }

        template <typename T>
        std::shared_ptr<AudioGraphNodeType<T>> CreateAudioNode() {
            if (!bInitialized) {
                spdlog::error("AudioSubsystem is not initialized. Cannot create audio node.");
                return nullptr;
            }
            auto node = std::make_shared<AudioGraphNodeType<T>>();
            node->Data.SetSampleRate(settings.SampleRate);
            node->Data.SetChannels(settings.AudioChannels);
            return node;
        }

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

        [[nodiscard]] uint32_t GetDefaultOutputDeviceID() const {
            return rtAudio->getDefaultOutputDevice();
        }

        void SelectOutputAudioDevice(uint32_t deviceID = 0);

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
        bool bInitialized{false};

        AudioGraphNodePtr<AudioFanInMixerNode> mainMixNode{nullptr};
        AudioSubsystemSettings settings{};
        std::unique_ptr<RtAudio> rtAudio{nullptr};
        const AudioDevice* currentOutputDevice{nullptr};
        std::vector<AudioDevice> audioDevices{};
    };
}
