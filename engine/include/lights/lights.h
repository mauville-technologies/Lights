//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <cstdint>

constexpr uint8_t VERSION_MAJOR = 0;
constexpr uint8_t VERSION_MINOR = 1;

/*
 * Lights Engine Architecture
 *
 * The engine is organized into two main layers:
 *
 * 1. CORE (lights/core/*)
 *    Low-level primitives and building blocks for direct manipulation and experimentation.
 *    - algo/       : Graph nodes and algorithms
 *    - audio/      : Audio subsystem and audio processing nodes
 *    - platform/   : Window management, input handling (GLFW, SDL3)
 *    - rendering/  : Rendering primitives (buffers, shaders, textures, materials)
 *    - text/       : Font loading and text rendering
 *    - util/       : Utilities (image loading, ring buffers, crypto, etc.)
 *
 *    Use core headers when you need fine-grained control over engine systems.
 *    Example: #include <lights/core/rendering/shader.h>
 * * 2. FRAMEWORK (lights/framework/*)
 *    High-level application-ready components built on core primitives.
 *    - game/       : Main game loop and configuration
 *    - scene/      : Scene management, layers, and resource management
 *    - input/      : Input subsystem abstraction
 *    - layers/     : Pre-built layer implementations (e.g., ClayUILayer)
 *
 *    Use framework headers for typical application development.
 *    Example: #include <lights/framework/game/game.h>
 *             #include <lights/framework/scene/scene.h>
 *
 * Most applications should start with framework headers and drop down to core
 * only when custom behavior is needed.
 */
