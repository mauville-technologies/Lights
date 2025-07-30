//
// Created by ozzadar on 2025-07-24.
//

#pragma once
#include <filesystem>
#include <lights/audio/audio_params.h>

#include "lights/audio/audio_graph.h"

namespace OZZ::lights::audio {
    enum class AudioCueContainerType {
        Wav,
        Ogg,
        Mp3,
        Flac,
        Unknown
    };

    enum class AudioCueLoadResult {
        Success,
        FileNotFound,
        UnsupportedFormat,
        UnknownError
    };

    enum class AudioCuePlayState {
        Playing,
        Paused,
        Stopped,
        Error
    };

    enum class AudioCueLoopMode {
        None,
        Loop,
        PingPong
    };

    struct AudioCueParams {
        AudioCueContainerType Type{AudioCueContainerType::Unknown};
        AudioCueLoadResult LoadResult{AudioCueLoadResult::UnknownError};
        AudioCuePlayState PlayState{AudioCuePlayState::Stopped};
        AudioCueLoopMode LoopMode{AudioCueLoopMode::None};
    };

    struct AudioCueAudioData {
        // It's assumed that the audio data is in normalized float format
        std::vector<float> Data;
        uint32_t SampleRate{44100};
        uint8_t BitDepth{2};
        size_t PlayHead{0};
        std::string FileName;

        [[nodiscard]] bool IsValid() const {
            return !Data.empty() && SampleRate > 0 && BitDepth > 0;
        }
    };

    class AudioCue : public AudioGraphNode {
    public:
        ~AudioCue() override;

        // Interface functions
        [[nodiscard]] std::string GetName() const override;
        bool Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) override;
        [[nodiscard]] std::vector<float> GetRenderedAudio() const override;
        [[nodiscard]] std::string GetDescription() const override;

        void Load(const std::filesystem::path& filePath);

        [[nodiscard]] bool IsValid() const {
            bool isValid = false;
            isValid &= !filePath.empty();
            isValid &= Params.LoadResult == AudioCueLoadResult::Success;
            isValid &= Params.Type != AudioCueContainerType::Unknown;
            isValid &= Params.PlayState != AudioCuePlayState::Error;
            return isValid;
        }

        [[nodiscard]] const std::filesystem::path& GetFilePath() const {
            return filePath;
        }

    private:
        void loadWav(const std::filesystem::path& filePath);

    public:
        AudioCueParams Params;

    private:
        std::filesystem::path filePath;
        AudioCueAudioData audioData;
    };
}
