//
// Created by ozzadar on 2025-07-17.
//

#pragma once

// C++20 concept for audio filters
#include <concepts>
#include <functional>
#include <vector>
#include <cstdint>
#include <string>

#include "lights/audio/audio_params.h"

namespace OZZ::lights::audio {
    class AudioProcessor {
    public:
        explicit AudioProcessor(const std::string& inName) : name(inName) {}
        virtual ~AudioProcessor() = default;

        [[nodiscard]] std::string GetName() const {
            return name;
        };

        virtual bool Apply(const AudioParams& params, std::vector<float>& audioStream) const = 0;
        [[nodiscard]] virtual std::string GetFilterName() const = 0;
        [[nodiscard]] virtual std::string GetDescription() const = 0;

    private:
        std::string name;
    };

    template <typename T>
    concept IsAudioProcessor = std::derived_from<T, AudioProcessor>;
}
