//
// Created by ozzadar on 2025-07-17.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "audio_params.h"

namespace OZZ::lights::audio {
    class AudioProcessor;

    class AudioSubmix {
    public:
        explicit AudioSubmix() = default;

        // we're going to assume that the audio stream is in normalized-float format
        [[nodiscard]] std::vector<float> MixAudio() const;

        template <typename T>
        std::weak_ptr<T> AddPreProcessor(const std::string& name) requires std::derived_from<T, AudioProcessor> {
            return preProcessors.emplace_back(std::make_shared<T>(name));
        }

        template <typename T>
        std::weak_ptr<T> AddPostProcessor(const std::string& name) requires std::derived_from<T, AudioProcessor> {
            return postProcessors.emplace_back(std::make_shared<T>(name));
        }

        bool RemovePreProcessor(const std::string& name);
        bool RemovePostProcessor(const std::string& name);

    private:

    private:
        std::string name;
        AudioParams params;

        std::vector<std::shared_ptr<AudioProcessor>> preProcessors;
        std::vector<std::shared_ptr<AudioProcessor>> postProcessors;

        std::vector<std::shared_ptr<AudioSubmix>> submixes;
    };
} // OZZ
