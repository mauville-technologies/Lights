//
// Created by ozzadar on 2025-07-17.
//

#include <ranges>
#include <lights/audio/audio_submix.h>

#include "lights/audio/filters/audio_filter.h"
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
}
