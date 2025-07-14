//
// Created by ozzadar on 2025-07-13.
//

#pragma once

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

    private:
        void detectAudioDevices();

    private:
        std::unique_ptr<RtAudio> rtAudio{nullptr};
        std::vector<AudioDevice> audioDevices;
    };
}
