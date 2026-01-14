//
// Created by ozzadar on 2025-07-27.
//

#pragma once
#include <string>

#include "lights/algo/graph_node.h"

namespace OZZ::lights::audio {
    class AudioGraphNode : public GraphNode {
    public:
        virtual ~AudioGraphNode() = default;

        void SetSampleRate(uint16_t sampleRate) {
            this->sampleRate = sampleRate;
        }

        [[nodiscard]] uint16_t GetSampleRate() const {
            return sampleRate;
        }

        void SetChannels(const uint8_t channels) {
            this->channels = channels;
        }

        [[nodiscard]] uint8_t GetChannels() const {
            return channels;
        }

        [[nodiscard]] virtual std::string GetName() const {
            return "Base AudioGraphNode";
        }

        virtual bool Render(int nFrames) {
            return false;
        };

        [[nodiscard]] const std::vector<float> &GetRenderedAudio() {
            return AudioGraphNode::renderedAudio;
        };

        [[nodiscard]] virtual std::string GetDescription() const {
            return "This node shouldn't be used directly";
        };

    protected:
        std::vector<float> renderedAudio{};

    private:
        uint16_t sampleRate{44100};
        uint8_t channels{2};
    };


    inline void ConnectAudioGraphNodes(AudioGraphNode *from, AudioGraphNode *to) {
        GraphNode::Connect(from, to);
    }
}
