//
// Created by ozzadar on 2025-08-04.
//

#include <ozz_audio/audio.h>
#include <fstream>
#include <samplerate.h>

namespace OZZ::audio {
    std::pair<AudioContext, std::vector<float>> Loader::LoadAudioFile(
        const std::filesystem::path& filePath) {
        const auto fileData = readFile(filePath);

        if (fileData.empty()) {
            return {{}, std::vector<float>()};
        }

        return parseFile(fileData);
    }

    std::vector<float> Loader::LoadAudioFileWithContext(const std::filesystem::path& filePath,
        const AudioContext targetContext) {
        auto [context, audioData] = LoadAudioFile(filePath);
        if (context == targetContext) {
            return audioData; // No conversion needed
        }

        if (context.Channels != targetContext.Channels) {
            //TODO: @paulm - Handle channel conversion
            return {};
        }

        const auto conversionRatio = static_cast<double>(targetContext.SampleRate) / context.SampleRate;
        const auto inputFrames = static_cast<long>(audioData.size() / context.Channels);
        const auto outputFrames = static_cast<long>(std::lround(inputFrames * conversionRatio));
        std::vector<float> convertedAudioData(outputFrames * targetContext.Channels);

        SRC_DATA srcData{
            .data_in = audioData.data(),
            .data_out = convertedAudioData.data(),
            .input_frames = inputFrames,
            .output_frames = outputFrames,
            .src_ratio = conversionRatio
        };

        if (const auto error = src_simple(&srcData, SRC_SINC_BEST_QUALITY, targetContext.Channels); error != 0) {
            return {};
        }

        return convertedAudioData;
    }

    std::vector<uint8_t> Loader::readFile(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            return {};
        }

        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return {};
        }

        // read the entire file into a buffer
        std::vector<uint8_t> buffer(
            (std::istreambuf_iterator(file)),
            std::istreambuf_iterator<char>()
        );

        file.close();
        return buffer;
    }

    std::pair<AudioContext, std::vector<float>> Loader::parseFile(const std::vector<uint8_t>& fileData) {
        switch (determineFileType(fileData)) {
            case AudioFileType::Wav:
                return parseWav(fileData);
            default:
                return {};
        }
    }

    AudioFileType Loader::determineFileType(const std::vector<uint8_t>& fileData) {
        if (fileData.size() < 4) {
            return AudioFileType::Unknown;
        }

        // Check for WAV file signature
        if (fileData[0] == 'R' && fileData[1] == 'I' && fileData[2] == 'F' && fileData[3] == 'F') {
            return AudioFileType::Wav;
        }

        return AudioFileType::Unknown;
    }

    std::pair<AudioContext, std::vector<float>> Loader::parseWav(const std::vector<uint8_t>& fileData) {
        // let's look for 'WAVE'
        if (fileData.size() < 44 || fileData[8] != 'W' || fileData[9] != 'A' || fileData[10] != 'V' || fileData[11] !=
            'E') {
            return {}; // Invalid WAV file
        }

        AudioContext context;
        context.FileType = AudioFileType::Wav;

        // crawl forward and start looking for the 'fmt ' chunk
        size_t startFmtChunkOffset = 12; // Start after 'RIFF' and 'WAVE'

        // loop through chunks to find 'fmt ' chunk
        while (startFmtChunkOffset < fileData.size()) {
            // get the chunk ID
            const auto chunkID = std::string(reinterpret_cast<const char*>(fileData.data() + startFmtChunkOffset), 4);
            const auto chunkSize = *reinterpret_cast<const uint32_t*>(fileData.data() + startFmtChunkOffset + 4);
            if (chunkID == "fmt ") {
                break; // Found the 'fmt ' chunk
            }
            startFmtChunkOffset += 8 + chunkSize; // Move to the next chunk
        }

        const auto fmtChunkDataOffset = startFmtChunkOffset + 8;
        const auto audioFormat = fileData[fmtChunkDataOffset] | (fileData[fmtChunkDataOffset + 1] << 8);
        context.Channels = fileData[fmtChunkDataOffset + 2] | (fileData[fmtChunkDataOffset + 3] << 8);
        context.SampleRate = fileData[fmtChunkDataOffset + 4] | (fileData[fmtChunkDataOffset + 5] << 8) | (fileData[
            fmtChunkDataOffset + 6] << 16) | (fileData[fmtChunkDataOffset + 7] << 24);

        const auto bitsPerSample = fileData[fmtChunkDataOffset + 14] | (fileData[fmtChunkDataOffset + 15] << 8);
        const auto bytesPerSample = bitsPerSample / 8;

        std::vector<float> audioData;
        switch (audioFormat) {
            case 1: {
                for (auto i = 44; i < fileData.size(); i += bytesPerSample) {
                    if (i + bytesPerSample > fileData.size()) {
                        break; // Prevent out-of-bounds access
                    }

                    switch (bytesPerSample) {
                        case 1: {
                            const uint8_t sample = fileData[i];
                            constexpr auto max = std::numeric_limits<uint8_t>::max();
                            audioData.push_back(
                                (static_cast<float>(sample) - (max / 2.f)) / (max / 2.f));
                            break;
                        }
                        case 2: {
                            const int16_t sample = *reinterpret_cast<const int16_t*>(fileData.data() + i);
                            audioData.push_back(static_cast<float>(sample) / std::numeric_limits<int16_t>::max());
                            break;
                        }
                        case 3: {
                            const int32_t sample = (fileData[i] | (fileData[i + 1] << 8) | (fileData[i + 2] << 16)) <<
                                8;
                            audioData.push_back(static_cast<float>(sample) / std::numeric_limits<int32_t>::max());
                            break;
                        }
                        case 4: {
                            const int32_t sample = *reinterpret_cast<const int32_t*>(fileData.data() + i);
                            audioData.push_back(static_cast<float>(sample) / std::numeric_limits<int32_t>::max());
                            break;
                        }
                        case 8: {
                            const int64_t sample = *reinterpret_cast<const int64_t*>(fileData.data() + i);
                            audioData.push_back(static_cast<float>(sample) / std::numeric_limits<int64_t>::max());
                            break;
                        }
                        default:
                            break;
                    }
                }
                return {context, audioData};
            }
            case 3: {
                if (bytesPerSample != 4 && bytesPerSample != 8) {
                    return {}; // Unsupported bit depth
                }
                // TODO: @paulm -- we're assuming the floats are normalized to [-1.0, 1.0]
                if (bytesPerSample == 4) {
                    audioData.resize((fileData.size() - 44) / sizeof(float));
                    memcpy(audioData.data(), fileData.data() + 44, fileData.size() - 44);
                }
                else {
                    // we need to convert 64-bit float to 32-bit float
                    for (auto i = 44; i < fileData.size(); i += bytesPerSample) {
                        if (i + 8 > fileData.size()) {
                            break; // Prevent out-of-bounds access
                        }
                        const double sample = *reinterpret_cast<const double*>(fileData.data() + i);
                        audioData.push_back(static_cast<float>(sample));
                    }
                }
                return {context, audioData};
            }
            default:
                return {}; // Unsupported audio format
        }
    }
}
