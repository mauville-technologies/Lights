//
// Created by ozzadar on 2025-07-17.
//

#include <ranges>
#include <lights/audio/audio_submix.h>

#include "lights/audio/filters/audio_processor.h"
#include "spdlog/spdlog.h"

namespace OZZ::lights::audio {
    std::vector<float> AudioSubmix::MixAudio() const {
        // TODO: @paulm - Apply sound cues for actually sampling data
        // TODO: @paulm - I think I need a way to tell this function the time range for which to mix audio
        std::vector<float> mixedAudio;
        for (const auto& preProcessor : preProcessors) {
            if (preProcessor) {
                if (!preProcessor->Apply(params, mixedAudio)) {
                    // Handle failure to apply pre-processor
                    spdlog::error("Failed to apply pre-processor: {}", preProcessor->GetName());
                }
            }
        }

        for (const auto& submix : submixes) {
            if (!submix) continue;

            auto submixAudio = submix->MixAudio();
            for (auto sample : std::ranges::views::zip(mixedAudio, submixAudio)) {
                std::get<0>(sample) += std::get<1>(sample);
            }

            for (float& sample : submixAudio) {
                // Normalize the submix audio to prevent clipping
                sample /= static_cast<float>(submixes.size());
                sample = std::tanh(sample); // Apply soft clipping
            }
        }

        for (const auto& postProcessor : postProcessors) {
            if (postProcessor) {
                if (!postProcessor->Apply(params, mixedAudio)) {
                    // Handle failure to apply post-processor
                    spdlog::error("Failed to apply post-processor: {}", postProcessor->GetName());
                }
            }
        }
        return mixedAudio;
    }

    std::weak_ptr<AudioSubmix> AudioSubmix::GetSubmix(const std::string& name) {
        const auto it = std::ranges::find_if(submixes,
                                             [&name](const auto& submix) {
                                                 return submix && submix->GetName() == name;
                                             });

        if (it != submixes.end()) {
            return *it; // Return the existing submix
        }
        return std::weak_ptr<AudioSubmix>{};
    }

    std::weak_ptr<AudioSubmix> AudioSubmix::GetOrCreateSubmix(const std::string& name, AudioParams&& inParams) {
        if (auto existingSubmix = GetSubmix(name); !existingSubmix.expired()) {
            return existingSubmix; // Return the existing submix if it exists
        }

        auto* newSubmix = new AudioSubmix(std::move(inParams));
        return submixes.emplace_back(newSubmix);
    }

    bool AudioSubmix::RemoveSubmix(const std::string& name) {
        const auto removeIt = std::erase_if(submixes, [&name](const auto& submix) {
            return submix && submix->GetName() == name;
        });

        return removeIt > 0;
    }

    bool AudioSubmix::RemovePreProcessor(const std::string& name) {
        return std::erase_if(preProcessors, [&name](const auto& processor) {
            return processor->GetName() == name;
        }) > 0;
    }

    bool AudioSubmix::RemovePostProcessor(const std::string& name) {
        return std::erase_if(postProcessors, [&name](const auto& processor) {
            return processor->GetName() == name;
        }) > 0;
    }

    std::weak_ptr<AudioSubmix> AudioSubmix::operator[](const std::string& name) {
        return GetSubmix(name);
    }

    AudioSubmix::AudioSubmix(AudioParams&& inParams) : params(inParams) {}
}
