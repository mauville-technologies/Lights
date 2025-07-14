//
// Created by ozzadar on 2025-07-13.
//

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "RtAudio.h"

namespace OZZ::lights::audio
{
    enum class EAudioDeviceType {
        Unknown,
        Input,
        Output,
    };

    struct AudioDevice {
        std::string Name;
        EAudioDeviceType Type;
        std::string ID;
    };

    class AudioSubsystem {
    public:
        void Init();
        void Shutdown();

    private:
        void detectAudioDevices();

    private:
        std::unique_ptr<RtAudio> rtAudio {nullptr};
        std::vector<AudioDevice> audioDevices;
    };
}
