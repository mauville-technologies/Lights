//
// Created by ozzadar on 2025-07-30.
//

#include <lights/audio/nodes/saw_tooth_node.h>
#include <cmath>

namespace OZZ::lights::audio {
    bool SawToothNode::Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) {
        constexpr auto numChannels = 2;
        const auto renderedAudioSize = nFrames * numChannels;

        renderedAudio.clear();
        renderedAudio.resize(renderedAudioSize);

        // Calculate phase increment per sample
        const float phaseIncrement = frequency / static_cast<float>(GetSampleRate());

        // Generate sawtooth wave
        for (int i = 0; i < nFrames; ++i) {
            // Generate one sample of sawtooth wave (-1 to 1)
            const float sample = (2.0f * phase) - 1.0f;

            // Update phase
            phase += phaseIncrement;
            if (phase >= 1.0f) {
                phase -= 1.0f;
            }

            // Write to all channels
            for (auto channel = 0; channel < numChannels; ++channel) {
                renderedAudio[i * numChannels + channel] = sample * 0.05f;
            }
        }

        return true;
    }

    void SawToothNode::SetNote(Note note, Octave octave) {
        currentNote = note;
        currentOctave = octave;
        frequency = CalculateFrequency(note, octave);
    }

    float SawToothNode::CalculateFrequency(Note note, Octave octave) const {
        // A4 = 440Hz (A in octave 4)
        constexpr float A4_FREQ = 440.0f;

        // Calculate semitones from A4
        int octaveDiff = static_cast<int>(octave) - static_cast<int>(Octave::Oct4);
        int noteDiff = static_cast<int>(note) - static_cast<int>(Note::C);
        int semitonesFromA4 = octaveDiff * 12 + noteDiff;

        // Calculate frequency using equal temperament formula: f = 440 * 2^(n/12)
        return A4_FREQ * std::pow(2.0f, static_cast<float>(semitonesFromA4) / 12.0f);
    }
}
