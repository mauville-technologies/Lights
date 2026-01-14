//
// Created by ozzadar on 2025-07-24.
//

#pragma once
#include <filesystem>
#include <lights/audio/audio_params.h>

#include "lights/audio/audio_graph.h"

namespace OZZ::lights::audio {
    enum class AudioCuePlayState {
        Playing,
        Stopped
    };

    enum class AudioCueLoopMode {
        None,
        Loop,
        PingPong
    };

    struct AudioCueAudioData {
        // It's assumed that the audio data is in normalized float format
        std::vector<float> Data;
        uint32_t SampleRate{44100};
        size_t PlayHead{0};
        std::string FileName;

        [[nodiscard]] bool IsValid() const {
            return !Data.empty() && SampleRate > 0;
        }
    };

    class AudioCue final : public AudioGraphNode {
    public:
        ~AudioCue() override;

        // Interface functions
        [[nodiscard]] std::string GetName() const override;

        bool Render(int nFrames) override;

        [[nodiscard]] std::string GetDescription() const override;

        bool Load(const std::filesystem::path &filePath);

        [[nodiscard]] bool IsValid() const {
            bool isValid = true;
            isValid &= !filePath.empty();
            isValid &= audioData.IsValid();
            return isValid;
        }

        [[nodiscard]] const std::filesystem::path &GetFilePath() const {
            return filePath;
        }

        float GetDuration() const {
            if (!audioData.IsValid()) {
                return 0.0f;
            }
            return static_cast<float>(audioData.Data.size()) / (audioData.SampleRate * GetChannels());
        }

        void Seek(const float seconds) {
            if (!audioData.IsValid()) {
                return;
            }
            const auto targetFrame = static_cast<size_t>(seconds * audioData.SampleRate * GetChannels());
            audioData.PlayHead = std::min(targetFrame, audioData.Data.size() - 1);
        }

    public:
        AudioCueLoopMode LoopMode{AudioCueLoopMode::None};
        AudioCuePlayState PlayState{AudioCuePlayState::Stopped};

    private:
        std::filesystem::path filePath;
        AudioCueAudioData audioData;

        bool bCurrentPlayDirectionForward{true};
    };
}
