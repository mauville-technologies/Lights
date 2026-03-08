# Third-Party Overview

This chapter summarizes third-party integration in Lights at a high level.

## Integration Model

Third-party dependencies are brought in through:
- local wrapper/build targets under `engine/third_party/*`;
- fetched dependencies via CMake `FetchContent`.

## Local Wrapper Targets (detailed)

### `engine/third_party/glad`
- static target wrapping GL loader source.
- used for OpenGL function loading.

### `engine/third_party/stb`
- static target wrapping stb image functionality.
- used by image loading/saving paths.

### `engine/third_party/clay`
- static target for Clay UI library integration.
- consumed by framework clay layer subsystem.

### `engine/third_party/asio`
- interface target for ASIO standalone headers;
- currently present as integration path, but not enabled in active third_party target list by default.

## Fetched Dependencies (high level)

Configured via `FetchContent` in `engine/third_party/CMakeLists.txt`:
- `spdlog`: logging
- `glm`: math types/transforms
- `freetype`: font rasterization
- `toml11`: TOML parsing
- `rtaudio`: audio device/stream backend
- `EnTT`: ECS utilities
- `nlohmann_json`: JSON parsing
- window backend dependency:
  - `glfw` when `OZZ_WINDOWING_SYSTEM=GLFW`
  - `SDL3` when `OZZ_WINDOWING_SYSTEM=SDL3`

Additional library-specific third-party:
- `libs/ozz_audio/third_party` fetches `libsamplerate`.

## Inferred Design Intent

- keep direct third-party usage centralized behind engine/library wrappers;
- make backend swaps compile-time configurable;
- avoid exposing third-party APIs to application code where possible.

## Speculative Direction (labeled)

Possible future changes:
- stricter boundary between external APIs and public engine headers;
- optional dependency profiles for leaner build variants;
- fuller enablement of currently optional wrapper targets.
