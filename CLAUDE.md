# CLAUDE.md — Lights

## Role

Game engine. Builds the `lights_engine` static library. C++23. Cascades the `LOCAL_RENDERING_DIR` and `OZZ_WINDOWING_SYSTEM` (`GLFW` | `SDL3`) configure flags down to `ozz_rendering`.

Normally consumed via `FetchContent` from `truck-kun/`. Standalone build is supported but not the production path.

For the broader stack picture see the per-repo CLAUDE.md files referenced from `D:\Projects\Game\CLAUDE.md`.

## Structure

```
Lights\
├── CMakeLists.txt              # set_base_dir() helper, OZZ_PLATFORM_* / OZZ_DEBUG defines,
│                               # add_subdirectory of libs\* and engine\
├── docker-compose.yml          # MongoDB 8.0 (port 27017) for backend / persistence work
├── README.md
├── docs\
├── scripts\
├── engine\
│   ├── CMakeLists.txt          # builds lights_engine static lib
│   ├── include\lights\         # PUBLIC headers (consumers include "lights/...")
│   │   ├── lights.h            # umbrella header
│   │   ├── core\
│   │   │   ├── algo\           # graph_node
│   │   │   ├── audio\          # audio_subsystem, audio_graph, audio_params,
│   │   │   │                   # nodes\ (audio_cue, audio_processor,
│   │   │   │                   #         audio_fan_in_mixer_node, saw_tooth_node)
│   │   │   ├── platform\       # window, platform_window
│   │   │   ├── rendering\      # buffer, material, render_target, renderable,
│   │   │   │                   # renderer, shader, texture, vertex, shapes,
│   │   │   │                   # renderables\ (renderable_viewport)
│   │   │   ├── text\           # font_loader
│   │   │   └── util\           # bytes, configuration, crypto, image, memory_literals,
│   │   │                       # mouse_utils, profiling, ring_buffer, state_machine, strings
│   │   └── framework\
│   │       ├── game\           # game.h
│   │       ├── input\          # input_subsystem, input_keys
│   │       ├── layers\clay\    # clay_ui_layer + components\ (clay_panel, clay_canvas_panel,
│   │       │                   # clay_component), clay_utils
│   │       └── scene\          # scene, scene_layer, scene_layer_manager, scene_object,
│   │                           # resource_manager, units, constants
│   ├── src\                    # mirrors include/lights/ tree;
│   │                           # platform\ has GLFW\ and SDL3\ implementations
│   │                           # (selected at configure time by OZZ_WINDOWING_SYSTEM)
│   └── third_party\            # FetchContent: spdlog, glm, freetype2, toml11, rtaudio,
│                               # GLFW or SDL3 (per OZZ_WINDOWING_SYSTEM), ozz_rendering,
│                               # tileson, clay
└── libs\
    ├── ozz_audio\              # audio runtime (own static lib)
    ├── ozz_binarypacking\      # binary serialization helpers
    ├── ozz_collision\          # collision primitives
    └── ozz_typegen\            # type / codegen helpers
```

## Build hygiene

- Build dir: `cmake-build-<variant>-claude`. `.gitignore` covers `cmake-build-*`, `build/`, `dist/`, `.idea/`, `.vs`.
- Standalone configure works but production path is FetchContent from `truck-kun/`. Prefer building from `truck-kun/` end-to-end.
- Redirect output to a file; read only on non-zero exit.

## Configure flags (when consumed via Lights's third_party)

- `OZZ_WINDOWING_SYSTEM` — `GLFW` (default) or `SDL3`. Selects platform layer in `engine/src/core/platform/`.
- `LOCAL_RENDERING_DIR` — path to a local `ozz_rendering` checkout; overrides the GitHub fetch.

## Compile defines

| Define | When |
|--------|------|
| `OZZ_PLATFORM_WINDOWS` | Windows builds |
| `OZZ_PLATFORM_MACOS`   | macOS builds |
| `OZZ_PLATFORM_LINUX`   | Linux builds |
| `OZZ_DEBUG`            | Debug builds (`CMAKE_BUILD_TYPE=Debug`) |

## Helper: `set_base_dir(target, base_dir)`

Defined in the root `CMakeLists.txt`. Routes a target's `RUNTIME_OUTPUT_DIRECTORY` and `ARCHIVE_OUTPUT_DIRECTORY` to:

```
<base_dir>\dist\<CMAKE_SYSTEM_NAME>\<CMAKE_BUILD_TYPE>\<target>\
```

Used by downstream consumers (`truck-kun`) to consolidate artifacts.

## Where to look first

| Area | Path |
|------|------|
| Public API surface | `engine/include/lights/lights.h` (umbrella) |
| RHI consumption | `engine/src/core/rendering/`, `engine/include/lights/core/rendering/` |
| Windowing / input | `engine/src/core/platform/{GLFW,SDL3}\`, `engine/src/framework/input/` |
| Scene graph | `engine/src/framework/scene/`, `engine/include/lights/framework/scene/` |
| Clay UI | `engine/src/framework/layers/clay/` |
| Audio | `engine/src/core/audio/` |
| Text / font | `engine/src/core/text/`, `engine/include/lights/core/text/` |
| Sub-libraries | `libs/ozz_audio/`, `libs/ozz_collision/`, `libs/ozz_binarypacking/`, `libs/ozz_typegen/` |
| Third-party setup | `engine/third_party/CMakeLists.txt` |

## Backend services

`docker-compose.yml` brings up MongoDB 8.0 with default creds `ozzadar / password`, port 27017, volume `db_volume`. Use only when work touches the persistence layer.
