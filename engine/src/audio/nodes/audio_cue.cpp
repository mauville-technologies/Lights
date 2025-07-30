//
// Created by ozzadar on 2025-07-24.
//

#include <lights/audio/nodes/audio_cue.h>

namespace OZZ::lights::audio {
    AudioCue::~AudioCue() {}

    std::string AudioCue::GetName() const {
        return "AudioCue - " + audioData.FileName;
    }

    bool AudioCue::Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) {
        return false;
    }

    std::vector<float> AudioCue::GetRenderedAudio() const {
        return {}; // Return empty vector for now, as this is a placeholder
    }

    std::string AudioCue::GetDescription() const {
        return R"(Audio Cue Node)
This node represents an audio cue that can be played in the audio graph.
It contains the audio data and can be connected to other nodes for processing.
)";
    }

    void AudioCue::Load(const std::filesystem::path& filePath) {}
}
