//
// Created by ozzadar on 2025-07-27.
//

#pragma once
#include "lights/algo/graphs/node.h"

namespace OZZ::lights::audio {
    class AudioGraphNode {
    public:
        virtual ~AudioGraphNode() = default;

        [[nodiscard]] virtual std::string GetName() const {
            return "Base AudioGraphNode";
        }

        virtual bool Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) {
            return false;
        };

        [[nodiscard]] virtual std::vector<float> GetRenderedAudio() const {
            return {};
        };

        [[nodiscard]] virtual std::string GetDescription() const {
            return "This node shouldn't be used directly";
        };
    };

    template <typename T>
    concept IsAudioProcessorNode = std::derived_from<T, AudioGraphNode>;

    template <IsAudioProcessorNode T>
    using AudioGraphNodeType = algo::Node<T>;

    template <IsAudioProcessorNode T>
    using AudioGraphNodePtr = std::shared_ptr<AudioGraphNodeType<T>>;

    template <IsAudioProcessorNode T>
    void ConnectAudioGraphNodes(AudioGraphNodePtr<T> from, AudioGraphNodePtr<T> to) {
        algo::Node<T>::Connect(from, to);
    }
}
