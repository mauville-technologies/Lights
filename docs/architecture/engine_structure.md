# Engine Structure

The Lights engine is organized into two main architectural layers to provide both flexibility for low-level
experimentation and convenience for rapid application development.

## Overview

```
lights/
├── core/          # Low-level primitives and building blocks
└── framework/     # High-level application-ready components
```

## Core Layer (`lights/core/`)

The core layer contains low-level primitives and building blocks that give you direct control over engine systems. Use
core headers when you need fine-grained control or want to experiment with custom implementations.

### Modules

#### `core/algo/`

Graph node system for building data flow graphs. Used internally by the rendering system.

- `graph_node.h` - Base graph node for building directed acyclic graphs

#### `core/audio/`

Low-level audio subsystem and processing nodes.

- `audio_subsystem.h` - Audio device management and real-time audio processing
- `audio_graph.h` - Audio processing graph
- `nodes/` - Audio processing nodes (cues, mixers, generators)

#### `core/platform/`

Platform abstraction layer for window management and input.

- `window.h` - Cross-platform window management
- `platform_window.h` - Platform-specific window interface
- GLFW and SDL3 backend implementations

#### `core/rendering/`

Low-level rendering primitives.

- `buffer.h` - Vertex and index buffer management
- `shader.h` - Shader compilation and management
- `texture.h` - Texture loading and management
- `material.h` - Material system combining shaders and textures
- `render_target.h` - Framebuffer and render target management
- `renderable.h` - Base renderable interface using graph nodes
- `renderer.h` - Main renderer orchestrating the render pipeline

#### `core/text/`

Font loading and text rendering utilities.

- `font_loader.h` - TrueType font loading

#### `core/util/`

General utility functions and data structures.

- `image.h` - Image loading utilities
- `ring_buffer.h` - Circular buffer implementation
- `configuration.h` - TOML-based configuration system
- `crypto.h` - Cryptographic utilities
- `enums.h` - Enum utilities
- `state_machine.h` - Generic state machine implementation

### Example Usage

```cpp
#include <lights/core/rendering/shader.h>
#include <lights/core/rendering/buffer.h>
#include <lights/core/platform/window.h>

// Create custom low-level rendering setup
auto shader = std::make_shared<OZZ::Shader>(shaderSource);
auto buffer = std::make_shared<OZZ::IndexVertexBuffer>();
buffer->UploadData(vertices, indices);
```

## Framework Layer (`lights/framework/`)

The framework layer provides high-level, application-ready components built on top of core primitives. This is what most
applications should use for rapid development.

### Modules

#### `framework/game/`

Main game loop and application framework.

- `game.h` - `LightsGame<SceneType>` template class that manages the entire game lifecycle
    - Window creation and management
    - OpenGL context initialization
    - Audio subsystem initialization
    - Input handling
    - Renderer setup
    - Fixed timestep game loop

#### `framework/scene/`

Scene management system with layers.

- `scene.h` - Base scene class with layer management
- `scene_layer.h` - Layer interface for organizing game systems
- `scene_layer_manager.h` - Layer lifecycle and ordering management
- `resource_manager.h` - Asset and resource management
- `scene_object.h` - Basic scene object representation

#### `framework/input/`

High-level input abstraction.

- `input_subsystem.h` - Unified input handling for keyboard, mouse, and gamepad
- `input_keys.h` - Key code definitions

#### `framework/layers/`

Pre-built layer implementations for common tasks.

- `clay/` - Clay UI integration
    - `clay_ui_layer.h` - Ready-to-use UI layer using Clay immediate mode UI
    - `components/` - Pre-built UI components (panels, canvases)

### Example Usage

```cpp
#include <lights/framework/game/game.h>
#include <lights/framework/scene/scene.h>

class MyScene : public OZZ::scene::Scene {
public:
    void InitScene(std::shared_ptr<OZZ::InputSubsystem> input,
                   OZZ::scene::ResourceManager* resourceManager) override {
        Scene::InitScene(input, resourceManager);
        
        // Add layers, load resources, etc.
        auto* gameLayer = layerManager->LoadLayer<GameLayer>("game");
    }
    
    OZZ::Renderable* GetSceneGraph() const override {
        // Return your scene graph root
    }
};

int main() {
    OZZ::game::LightsGame<MyScene> game{configPath};
    game.Run();
}
```

## Import Guidelines

### For Application Development

Start with framework headers for quick development:

```cpp
#include <lights/framework/game/game.h>
#include <lights/framework/scene/scene.h>
#include <lights/framework/input/input_subsystem.h>
```

### For Custom Systems

Drop down to core when you need custom behavior:

```cpp
#include <lights/framework/scene/scene_layer.h>
#include <lights/core/rendering/shader.h>
#include <lights/core/rendering/material.h>
```

### For Engine Development

Use core headers when implementing new engine features:

```cpp
#include <lights/core/rendering/renderable.h>
#include <lights/core/algo/graph_node.h>
```

## Design Philosophy

1. **Core is for primitives**: The core layer provides building blocks without opinions about how you use them. It's
   designed for flexibility and direct control.

2. **Framework is for applications**: The framework layer provides opinionated, high-level abstractions that handle
   common game development patterns (game loops, scene management, layers).

3. **No hidden magic**: Even when using framework, all core systems remain accessible. You can always drop down to core
   for custom behavior.

4. **Composition over inheritance**: Both layers favor composition. The scene layer system, graph nodes, and component
   systems all support composable architectures.

## Migration from Old Structure

Old includes automatically map to the new structure:

- `lights/rendering/*` → `lights/core/rendering/*`
- `lights/scene/*` → `lights/framework/scene/*`
- `lights/game/*` → `lights/framework/game/*`
- `lights/input/*` → `lights/framework/input/*`
- `lights/library/layers/*` → `lights/framework/layers/*`
- `lights/util/*` → `lights/core/util/*`
- etc.

