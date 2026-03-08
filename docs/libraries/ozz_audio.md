# Library: ozz_audio

Path: `libs/ozz_audio/*`

## Purpose

`ozz_audio` is an audio file loading/conversion utility library used by engine audio nodes (notably `AudioCue`).

## Key Types

- `AudioFileType` (Wav/Ogg/Mp3/Flac/Unknown)
- `AudioContext`
  - file type
  - sample rate
  - channel count
- `Loader`
  - `LoadAudioFile(path)`
  - `LoadAudioFileWithContext(path, targetContext)`

## Current Behavior (concrete)

- file bytes are read from disk and format is detected;
- WAV parsing is implemented with PCM/floating-format handling;
- resampling to target sample rate uses libsamplerate (`src_simple`);
- if channel conversion is required, current implementation returns empty result.

## Inferred Design Intent

- isolate audio file decode/transcode concerns outside the main engine runtime layer;
- feed normalized float PCM buffers into runtime audio graph nodes;
- support context-driven conversion for runtime playback consistency.

## Speculative Direction (labeled)

Potential future evolution:
- robust multi-format decode beyond current WAV-focused path;
- proper channel up/down-mixing;
- streaming decode paths for larger media.
