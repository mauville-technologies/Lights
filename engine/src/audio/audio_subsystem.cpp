//
// Created by ozzadar on 2025-07-13.
//

#include <lights/audio/audio_subsystem.h>

#include <samplerate.h>
#include "spdlog/spdlog.h"

namespace OZZ::lights::audio {
    void AudioSubsystem::Init(AudioSubsystemSettings &&inSettings) {
        spdlog::info("Initializing Audio Subsystem...");

        settings = std::move(inSettings);
        initializeRtAudio();
        initializeMainMix();
        if (!rtAudio) {
            spdlog::error("Failed to initialize RtAudio. Audio subsystem will not function.");
            return;
        }
        if (!mainMixNode) {
            spdlog::error("Failed to initialize main mix node. Audio subsystem will not function.");
            return;
        }
        spdlog::info("Audio Subsystem initialized with sample rate: {}, channels: {}",
                     settings.SampleRate, settings.AudioChannels);
        bInitialized = true;
    }

    void AudioSubsystem::Shutdown() {
        spdlog::info("Shutting down Audio Subsystem...");
        shutdownMainMix();
        shutdownRtAudio();
    }

    void AudioSubsystem::SelectOutputAudioDevice(const uint32_t deviceID) {
        const auto *device = GetAudioDevice(deviceID);
        if (!device) {
            spdlog::error("Audio device with ID {} not found.", deviceID);
            return;
        }

        if (device == currentOutputDevice) {
            spdlog::info("Output device {} is already selected.", device->Name);
            return;
        }

        closeOpenStream();
        currentOutputDevice = nullptr;

        auto BufferSize = 256U; // Default buffer size
        RtAudio::StreamParameters OutParameters;
        OutParameters.deviceId = device->ID;
        OutParameters.nChannels = device->OutputChannels;
        OutParameters.firstChannel = 0;
        spdlog::info("Selected Output Device: {} (ID: {}, Channels: {})",
                     device->Name, device->ID, device->OutputChannels);

        RtAudio::StreamOptions OutOptions{
            .flags = RTAUDIO_SCHEDULE_REALTIME | RTAUDIO_MINIMIZE_LATENCY,
            .numberOfBuffers = 1,
            .priority = 1
        };
        rtAudio->openStream(
            &OutParameters, // Output device ID
            nullptr, // No input
            RTAUDIO_FLOAT32, // Sample format
            settings.SampleRate, // Sample rate
            &BufferSize, // Buffer size
            [](void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime,
               RtAudioStreamStatus status, void *userData) {
                const auto *audioSubsystem = static_cast<AudioSubsystem *>(userData);
                return audioSubsystem->renderAudio(outputBuffer, inputBuffer, nFrames, streamTime, status);
            },
            this,
            &OutOptions
        );

        spdlog::info("Opening audio stream with buffer size: {}, sample rate: {}", BufferSize,
                     rtAudio->getStreamSampleRate());
        rtAudio->startStream();
        currentOutputDevice = device;
    }

    void AudioSubsystem::detectAudioDevices() {
        if (!rtAudio) {
            spdlog::error("RtAudio is not initialized. Cannot detect audio devices.");
            return;
        }
        auto devices = rtAudio->getDeviceIds();

        // scan through all devices
        for (auto id: devices) {
            auto deviceInfo = rtAudio->getDeviceInfo(id);
            spdlog::info("Device {}: {} (Input Channels: {}, Output Channels: {})",
                         id, deviceInfo.name, deviceInfo.inputChannels, deviceInfo.outputChannels);

            // store the device info
            if (deviceInfo.inputChannels > 0 || deviceInfo.outputChannels > 0) {
                AudioDevice device;
                device.ID = id;
                device.Name = deviceInfo.name;
                device.InputChannels = deviceInfo.inputChannels;
                device.OutputChannels = deviceInfo.outputChannels;
                audioDevices.push_back(device);
            }
        }
    }

    bool AudioSubsystem::initializeRtAudio() {
        rtAudio = std::make_unique<RtAudio>(RtAudio::Api::UNSPECIFIED,
                                            [this](auto ErrorType, const auto &ErrorMessage) {
                                                spdlog::error("RtAudio Error: {} - {}", static_cast<int>(ErrorType),
                                                              ErrorMessage);
                                            });

        detectAudioDevices();
        SelectOutputAudioDevice(GetDefaultOutputDeviceID()); // Default to the first available output device
        return true;
    }

    bool AudioSubsystem::initializeMainMix() {
        mainMixNode = std::make_shared<AudioFanInMixerNode>();
        return true;
    }

    int AudioSubsystem::renderAudio(void *outputBuffer, void *inputBuffer, unsigned int nFrames, double streamTime,
                                    RtAudioStreamStatus status) const {
        const auto deviceSampleRate = rtAudio->getStreamSampleRate();
        const auto deviceChannels = rtAudio->getDeviceInfo(currentOutputDevice->ID).outputChannels;

        // We need to ensure to render enough audio to fill the output buffer after resampling.
        const auto conversionRatio = deviceSampleRate / static_cast<double>(settings.SampleRate);
        const auto requiredFrames = static_cast<unsigned int>(nFrames / conversionRatio);
        const auto bufferSize = nFrames * deviceChannels;

        if (status == RTAUDIO_OUTPUT_UNDERFLOW) {
            spdlog::warn("Audio output underflow detected. This may cause audio glitches.");
        }
        if (status == RTAUDIO_INPUT_OVERFLOW) {
            spdlog::warn("Audio input overflow detected. This may cause audio glitches.");
        }

        // Ensure the output buffer is large enough
        if (outputBuffer == nullptr || requiredFrames == 0) {
            spdlog::error("Output buffer is null or nFrames is zero. Cannot render audio.");
            return 0;
        }

        // Flatten the main mix node to get dependency graph
        const auto sortedNodes = GraphNode::TopologicalSort(mainMixNode.get());
        if (!sortedNodes) {
            spdlog::error("Failed to flatten audio graph. Cycle detected or empty graph.");
            return 0;
        }

        for (auto &node: *sortedNodes) {
            auto CurrentNode = static_cast<AudioGraphNode *>(node);

            // TODO: Double check that inputs exist
            if (!CurrentNode->Render(requiredFrames)) {
                spdlog::warn("Node {} failed to render audio.", CurrentNode->GetName());
            }
        }

        // we can then grab the rendered audio from the main mix node
        // TODO: @paulm - i should probably be using views here instead of copying the data
        auto mix = mainMixNode->GetRenderedAudio();

        if (!mix.empty() && deviceSampleRate != settings.SampleRate) {
            // resample the audio to match the device sample rate
            spdlog::warn("Resampling audio from {} Hz to {} Hz. This may cause quality loss.",
                         settings.SampleRate, deviceSampleRate);

            // first we'll copy the mix to a temporary buffer
            const auto inMix = mix;
            const long inMixFrameCount = static_cast<long>(inMix.size() / settings.AudioChannels);

            // we'll then resize mix to the new sample rate
            const auto outMixFrameCount = static_cast<long>(nFrames);
            mix.resize(outMixFrameCount * deviceChannels);
            SRC_DATA srcData{
                .data_in = inMix.data(),
                .data_out = mix.data(),
                .input_frames = inMixFrameCount,
                .output_frames = outMixFrameCount,
                .src_ratio = conversionRatio,
            };

            if (const auto error = src_simple(&srcData, SRC_SINC_BEST_QUALITY, deviceChannels); error != 0) {
                spdlog::error("Error resampling audio: {}", src_strerror(error));
                return -1; // Return an error code
            }
        }

        if (mix.empty()) {
            spdlog::warn("Main mix node rendered no audio data. Returning silence.");
            mix = std::vector<float>(bufferSize, 0.0f);
        } else if (mix.size() > bufferSize) {
            spdlog::warn("Main mix node rendered more audio data than requested. Truncating.");
            // Truncate the mix if it's longer than nFrames
            mix.resize(bufferSize);
        } else if (mix.size() < bufferSize) {
            // pad with silence if the mix is shorter than nFrames
            mix.resize(bufferSize, 0.0f);
        }

        // TODO: @paulm - Handle different channel counts properly
        for (unsigned int i = 0; i < nFrames; ++i) {
            // Convert float to int16_t and write to output buffer
            static_cast<float *>(outputBuffer)[i * 2] = mix[i * 2];
            static_cast<float *>(outputBuffer)[i * 2 + 1] = mix[i * 2 + 1];
        }

        return 0;
    }

    void AudioSubsystem::shutdownMainMix() {
    }

    void AudioSubsystem::closeOpenStream() {
        if (rtAudio) {
            if (rtAudio->isStreamOpen()) {
                rtAudio->closeStream();
            }
        }
    }

    void AudioSubsystem::shutdownRtAudio() {
        audioDevices.clear();

        closeOpenStream();
        if (rtAudio) {
            rtAudio.reset();
            spdlog::info("Audio Subsystem shut down successfully.");
        }
    }
}
