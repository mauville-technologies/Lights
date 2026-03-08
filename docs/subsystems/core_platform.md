# Core Subsystem: Platform

Path: `engine/include/lights/core/platform/*`

## What It Contains

Platform abstracts windowing/input backend details behind a stable interface:
- high-level wrapper: `OZZ::Window`
- backend contract: `platform::IPlatformWindow`
- backend implementations:
  - GLFW (`core/platform/GLFW/*`)
  - SDL3 (`core/platform/SDL3/*`)

## Interface Shape

`IPlatformWindow` exposes:
- window lifecycle and context control;
- event polling;
- fullscreen/sizing/text-input mode controls;
- key/controller/mouse state snapshots.

Callbacks are grouped in `WindowCallbacks`:
- close/resize
- key/text input
- controller connect/disconnect
- mouse move/scroll

## Backend Selection

Backend is selected by compile definitions from CMake:
- `OZZ_GLFW`
- `OZZ_SDL3`

`Window::initWindow` instantiates the selected backend and initializes GLAD once.

## Inferred Design Intent

- keep backend dependencies isolated to platform folder;
- expose backend-neutral input state arrays/maps to framework input layer;
- centralize OS/window concerns away from game/scene code.

## Speculative Direction (labeled)

Potential future improvements:
- complete parity between GLFW and SDL3 feature paths;
- stronger context ownership APIs (currently `Window::MakeContextCurrent` is intentionally minimal);
- expanded platform events and device hotplug behavior.
