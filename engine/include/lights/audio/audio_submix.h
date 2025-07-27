//
// Created by ozzadar on 2025-07-17.
//

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "audio_params.h"
#include "filters/audio_processor.h"

namespace OZZ::lights::audio {
    class AudioProcessor;

    class AudioSubmix {
    public:
        // we're going to assume that the audio stream is in normalized-float format
        [[nodiscard]] std::vector<float> MixAudio() const;

        std::weak_ptr<AudioSubmix> GetSubmix(const std::string& name);
        /**
         * GetSubmix retrieves an existing submix by name or creates a new one if it doesn't exist.
         *
         * @param name Submix Name
         * @param inParams Parameters for the submix, such as sample rate and bit depth
         * @return the submix if it exists, or a new one if it doesn't.
         */
        std::weak_ptr<AudioSubmix> GetOrCreateSubmix(const std::string& name, AudioParams&& inParams);
        bool RemoveSubmix(const std::string& name);

        [[nodiscard]] const std::vector<std::shared_ptr<AudioSubmix>>& GetSubmixes() const {
            return submixes;
        }

        [[nodiscard]] bool HasSubmixes() const {
            return !submixes.empty();
        }

        [[nodiscard]] bool IsEmpty() const {
            return preProcessors.empty() && postProcessors.empty() && submixes.empty();
        }

        [[nodiscard]] const std::string& GetName() const {
            return name;
        }

        [[nodiscard]] const AudioParams& GetParams() const {
            return params;
        }

        template <typename T>
            requires IsAudioProcessor<T>
        std::weak_ptr<T> AddPreProcessor(const std::string& name) {
            return preProcessors.emplace_back(std::make_shared<T>(name));
        }

        template <typename T>
            requires IsAudioProcessor<T>
        std::weak_ptr<T> AddPostProcessor(const std::string& name) {
            return postProcessors.emplace_back(std::make_shared<T>(name));
        }

        bool RemovePreProcessor(const std::string& name);
        bool RemovePostProcessor(const std::string& name);

        // let's create map access functions for the submixes
        std::weak_ptr<AudioSubmix> operator[](const std::string& name);

    protected:
        friend class AudioSubsystem;
        explicit AudioSubmix(AudioParams&& inParams);

    private:
        std::string name;
        AudioParams params;

        std::vector<std::shared_ptr<AudioProcessor>> preProcessors;
        std::vector<std::shared_ptr<AudioProcessor>> postProcessors;

        std::vector<std::shared_ptr<AudioSubmix>> submixes;
    };
} // OZZ
