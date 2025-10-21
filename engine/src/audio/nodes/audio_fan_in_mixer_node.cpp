//
// Created by ozzadar on 2025-07-30.
//

#include <lights/audio/nodes/audio_fan_in_mixer_node.h>
#include <spdlog/spdlog.h>

namespace OZZ::lights::audio {
    bool AudioFanInMixerNode::Render(int nFrames) {
        constexpr auto numChannels = 2;
        const auto renderedAudioSize = nFrames * numChannels;

        renderedAudio.clear();
        renderedAudio.resize(renderedAudioSize, 0.0f);

        const auto scale = inputs.empty() ? 1.f : 1.f / static_cast<float>(inputs.size());

        for (auto *inputNode: inputs) {
            // we want to first ensure not null
            if (!inputNode) {
                spdlog::warn("Input node is null");
                continue;
            }

            auto *audioInputNode = static_cast<AudioGraphNode *>(inputNode);
            const auto inputRenderedAudio = audioInputNode->GetRenderedAudio();

            // for each float in rendered audio, sum to existing output
            for (auto i = 0; i < inputRenderedAudio.size(); i++) {
                if (i >= renderedAudioSize) {
                    spdlog::warn("Input Rendered audio frame index out of range");
                    break;
                }

                renderedAudio[i] += inputRenderedAudio[i];
            }
        }

        for (auto &frame: renderedAudio) {
            frame = scale * frame;
        }
        return true;
    }
}
