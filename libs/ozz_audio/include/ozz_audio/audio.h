//
// Created by ozzadar on 2025-08-04.
//

#pragma once
#include <cstdint>
#include <vector>
#include <filesystem>

namespace OZZ::audio {
    enum class AudioFileType : uint8_t {
        Unknown,
        Wav,
        Ogg,
        Mp3,
        Flac
    };

    struct AudioContext {
        AudioFileType FileType{AudioFileType::Unknown};
        uint16_t SampleRate{0};
        uint8_t Channels{0};

        bool IsValid() const {
            return
                FileType != AudioFileType::Unknown &&
                SampleRate > 0 &&
                Channels > 0;
        }

        bool operator==(const AudioContext& other) const {
            return FileType == other.FileType &&
                SampleRate == other.SampleRate &&
                Channels == other.Channels;
        }
    };

    class Loader {
    public:
        /**
         * Function to separate the header from the audio data in a file.
         * @param filePath The path to the audio file.
         * @return A pair containing the audio file type and the raw audio data in PCM16 float format.
         */
        static std::pair<AudioContext, std::vector<float>> LoadAudioFile(const std::filesystem::path& filePath);

        /**
         * Loads an audio file and converts it to match the target audio context.
         * @param filePath The path to the audio file.
         * @param targetContext The desired audio context (sample rate, channels, bit depth).
         * @return The audio data converted to the target context in PCM16 float format.
         */
        static std::vector<float> LoadAudioFileWithContext(
            const std::filesystem::path& filePath, AudioContext targetContext);

    private:
        static std::vector<uint8_t> readFile(const std::filesystem::path& filePath);

        /**
         * Parses the header of the audio file to extract metadata and audio data.
         * @param fileData The raw file data read from the audio file.
         * @return A pair containing the audio context (file type, sample rate, channels, bit depth) and the audio data in PCM16 float format.
         */
        static std::pair<AudioContext, std::vector<float>> parseFile(const std::vector<uint8_t>& fileData);

        static AudioFileType determineFileType(const std::vector<uint8_t>& fileData);
        static std::pair<AudioContext, std::vector<float>> parseWav(const std::vector<uint8_t>& fileData);
    };
}
