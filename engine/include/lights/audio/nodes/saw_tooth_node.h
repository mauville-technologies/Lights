//
// Created by ozzadar on 2025-07-30.
//

#pragma once
#include "lights/audio/audio_graph.h"

namespace OZZ::lights::audio {
    enum class Note {
        C, CSharp, D, DSharp, E, F, FSharp, G, GSharp, A, ASharp, B
    };

    enum class Octave {
        Oct0, Oct1, Oct2, Oct3, Oct4, Oct5, Oct6, Oct7, Oct8
    };

    class SawToothNode : public AudioGraphNode {
    public:
        ~SawToothNode() override = default;

        [[nodiscard]] std::string GetName() const override {
            return "SawTooth";
        }

        bool Render(int nFrames, const std::vector<AudioGraphNode*>& inputs) override;
        [[nodiscard]] std::vector<float> GetRenderedAudio() const override;

        [[nodiscard]] std::string GetDescription() const override {
            return "SawTooth synth";
        }

        void SetNote(Note note, Octave octave);

        [[nodiscard]] Note GetCurrentNote() const {
            return currentNote;
        }

        [[nodiscard]] Octave GetCurrentOctave() const {
            return currentOctave;
        }

    private:
        std::vector<float> renderedAudio;
        float phase = 0.0f;
        float frequency = 440.0f; // A4 note by default
        float sampleRate = 44100.0f;
        Note currentNote = Note::A;
        Octave currentOctave = Octave::Oct4;

        [[nodiscard]] float CalculateFrequency(Note note, Octave octave) const;
    };
} // audio
