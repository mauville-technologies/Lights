# Library: ozz_binarypacking

Path: `libs/ozz_binarypacking/*`

## Purpose

`ozz_binarypacking` provides small wrapper types for binary-oriented serialization and packet composition.

## Key API Surface

- `TypeIdentifier` enum for serialized type tags.
- typed wrappers in `types/*` implementing `std::span<uint8_t>` conversion.
- `Packet<T...>` tuple-based aggregate serialization in `binary.h`.

## Packet Model (concrete)

`Packet<T...>` behavior:
- requires each type to be convertible to `std::span<uint8_t>`;
- writes a small packet header (`TypeIdentifier::Packet` + payload size);
- concatenates element byte spans in order.

## Current Implementation State

Implemented:
- integer wrappers (`Int8/16/32/64`, `UInt8/16/32/64`);
- string wrapper.

Present but currently minimal/placeholder:
- floating wrapper file;
- boolean wrapper file.

## Inferred Design Intent

- lightweight schema-free binary value packing;
- explicit type tagging for decode paths;
- simple compatibility layer to support generated/network payload tooling.

## Speculative Direction (labeled)

Potential next steps:
- complete float/double/bool implementations;
- decode/parsing helpers for packet-to-typed structures;
- stronger endianness/versioning conventions.
