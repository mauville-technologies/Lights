//
// Created by ozzadar on 2025-07-18.
//

#pragma once

namespace OZZ::lights::audio {
    // Audio parameters for the audio subsystem
    struct AudioParams {
        uint32_t SampleRate{44100};
        uint8_t BitDepth{2};
    };
} // namespace OZZ::lights::audio
