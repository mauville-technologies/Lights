//
// Created by ozzadar on 2025-07-24.
//

#include <lights/audio/nodes/audio_cue.h>
#include <ozz_audio/audio.h>
#include <spdlog/spdlog.h>

namespace OZZ::lights::audio {
    AudioCue::~AudioCue() {
    }

    std::string AudioCue::GetName() const {
        return "AudioCue - " + audioData.FileName;
    }

    bool AudioCue::Render(int nFrames) {
        renderedAudio.clear();
        // Ensure the audio data is valid
        if (!IsValid()) {
            spdlog::error("AudioCue is not valid. Cannot render audio.");
            return false;
        }
        const auto framesToRender = std::min(
            nFrames, static_cast<int>(audioData.Data.size() - audioData.PlayHead) / GetChannels());
        if (PlayState == AudioCuePlayState::Stopped || audioData.Data.empty() || framesToRender <= 0) {
            // fill empty audio
            renderedAudio.resize(nFrames * GetChannels(), 0.0f);
            audioData.PlayHead = 0;
            return true;
        }
        renderedAudio.resize(nFrames * GetChannels(), 0.0f);
        // get the frames
        const auto startOffset = audioData.PlayHead;
        const auto bytesToRender = framesToRender * GetChannels();
        const auto endOffset = startOffset + bytesToRender;
        // copy the audio data to the rendered audio
        // TODO: @paulm - implement pingpong logic
        std::copy_n(audioData.Data.begin() + startOffset, bytesToRender, renderedAudio.begin());
        audioData.PlayHead = endOffset;
        // We've got audio data to render and we're currently playing
        if (framesToRender < nFrames) {
            switch (LoopMode) {
                // TODO: @paulm - implement pingpong logic
                case AudioCueLoopMode::PingPong:
                case AudioCueLoopMode::None: {
                    PlayState = AudioCuePlayState::Stopped;
                    // Fill the rest of the buffer with silence
                    renderedAudio.resize(nFrames * GetChannels(), 0.0f);
                    break;
                }
                case AudioCueLoopMode::Loop: {
                    // Loop the audio data
                    const auto remainingFrames = nFrames - framesToRender;
                    audioData.PlayHead = remainingFrames * GetChannels();
                    // play the audio data from the beginning
                    std::copy_n(audioData.Data.begin(), remainingFrames * GetChannels(),
                                renderedAudio.begin() + framesToRender * GetChannels());
                    break;
                }
                    // case AudioCueLoopMode::PingPong: {
                    //     const auto remainingFrames = nFrames - framesToRender;
                    //
                    //     break;
                    // }
            }
        }

        return true;
    }

    std::string AudioCue::GetDescription() const {
        return R"(Audio Cue Node)
This node represents an audio cue that can be played in the audio graph.
It contains the audio data and can be connected to other nodes for processing.
)";
    }

    bool AudioCue::Load(const std::filesystem::path &filePath) {
        const OZZ::audio::AudioContext context{
            .SampleRate = GetSampleRate(),
            .Channels = GetChannels(),
        };

        if (auto audio = OZZ::audio::Loader::LoadAudioFileWithContext(filePath, context); !audio.empty()) {
            audioData.Data = std::move(audio);
            audioData.SampleRate = context.SampleRate;
            audioData.FileName = filePath.filename().string();
            audioData.PlayHead = 0;
            this->filePath = filePath;

            return true;
        }
        return false;
    }
}
