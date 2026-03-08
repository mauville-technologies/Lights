# Core Subsystem: Util

Path: `engine/include/lights/core/util/*`

## What It Contains

The util subsystem is a collection of shared helpers used across core and framework code.

## Modules (concrete)

### `configuration.h`
- `Configuration<T>` template for TOML-backed config load/save.
- Requires `fromToml` and `toToml` contract on `T`.

### `image.h` / `image.cpp`
- image loading via stb_image;
- optional external buffer loading mode;
- atlas merge utility (`Image::MergeImages`);
- pixel flipping and PNG output.

### `ring_buffer.h` / `ring_buffer.cpp`
- byte ring buffer with allocate/consume semantics;
- supports internal ownership or external memory spans;
- used by GPU staging upload path.

### `state_machine.h`
- generic state machine template with:
  - enter/update/exit callbacks;
  - transition conditions;
  - enum-indexed state definition arrays.

### `enums.h`
- enum helpers (`to_index`, `from_index`);
- opt-in bitmask operator support.

### Other helpers
- `bytes.h`: byte extraction helpers.
- `strings.h`: tokenize/lowercase utilities.
- `mouse_utils.h`: coordinate conversion helpers.
- `memory_literals.h`: `_KiB`, `_MiB`, `_GiB`.
- `crypto.h`: hash/salt helper wrappers around Crypto++.

## Inferred Design Intent

- keep reusable low-level mechanics centralized;
- avoid duplicated small utilities in gameplay/engine layers;
- provide lightweight building blocks rather than heavyweight frameworks.

## Speculative Direction (labeled)

Potential evolution:
- tighter validation/error handling for utility boundaries;
- expansion of serialization/string/byte helpers;
- stronger async/resource support primitives as engine workloads grow.
