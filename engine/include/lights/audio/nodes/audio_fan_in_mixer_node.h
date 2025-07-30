//
// Created by ozzadar on 2025-07-30.
//

#pragma once
#include <lights/audio/audio_graph.h>

namespace OZZ::lights::audio {
    class AudioFanInMixerNode final : public AudioGraphNode {
    public:
        ~AudioFanInMixerNode() override = default;

        [[nodiscard]] std::string GetName() const override {
            return "Audio Fan-In Mixer";
        }

        bool Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) override;

        [[nodiscard]] std::vector<float> GetRenderedAudio() const override;

        [[nodiscard]] std::string GetDescription() const override {
            return "Mixes multiple audio inputs into a single output stream.";
        }

    private:
        std::vector<float> renderedAudio;
    };
} // OZZ

