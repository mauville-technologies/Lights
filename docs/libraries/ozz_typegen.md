# Library: ozz_typegen

Path: `libs/ozz_typegen/*`

## Purpose

`ozz_typegen` is a schema-to-C++ code generator intended for low-friction generated types without full protobuf-style toolchain complexity.

## Tooling Shape

- generator binary target: `ozz_typegen`
- core parser/generator in `src/generator.*`
- CMake helper function:
  - `generate_ozz_source(INPUT_FILE OUTPUT_FILE)`

## Supported Definition Concepts (concrete)

Current parser supports top-level:
- `struct` definitions
- `enum` definitions

Supported field kinds:
- primitive-like: `int`, `float`, `double`, `bool`, `string`
- custom generated type references
- `array` modifier for vector-like fields

Generated output includes:
- header prologue + includes
- generated `struct`/`enum` declarations
- references to `ozz_binary` include path conventions

## Known Limitations

From source and library README:
- top-level definitions only;
- cross-file generated references are not a primary path yet;
- C++ generation focus only.

## Inferred Design Intent

- provide pragmatic schema generation tightly aligned with project conventions;
- keep build integration CMake-native and lightweight;
- support networking/data-model workflows without heavy dependencies.

## Speculative Direction (labeled)

Likely growth:
- richer type system and validation diagnostics;
- improved inter-file references/namespace handling;
- optional generated serializers/deserializers.
