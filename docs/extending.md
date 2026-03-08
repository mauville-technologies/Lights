# Extending the Engine

Lights is built to be extended through composition around scenes, layers, renderables, and utility systems.

## Extension Surfaces

### Framework-level extensions
- New `Scene` subclasses for lifecycle and state orchestration.
- New `SceneLayer` subclasses for gameplay/UI/system slices.
- Input behavior through additional action/chord/axis mappings.

### Core-level extensions
- New `Renderable` nodes and graph wiring.
- New audio graph nodes (`AudioGraphNode` derivatives).
- New utility/system primitives where needed.

## Pattern: Add a Custom Layer

```cpp
#include <lights/framework/scene/scene_layer.h>

class MyLayer final : public OZZ::scene::SceneLayer, public OZZ::Renderable {
public:
    std::string GetRenderableName() override { return "MyLayer"; }
    void Init() override {}
    void Tick(float dt) override {}
    void RenderTargetResized(glm::ivec2 size) override {}

protected:
    bool render() override { return true; }
};
```

Then register from scene:

```cpp
auto* layer = layerManager->LoadLayer<MyLayer>("MyLayer");
layerManager->SetLayerActive("MyLayer", true);
```

## Pattern: Add a Render Graph Node

Use `Renderable` + `GraphNode::Connect` to integrate passes explicitly:

```cpp
GraphNode::Connect(sourceRenderable, myCustomPass);
GraphNode::Connect(myCustomPass, finalSceneRenderable);
```

## Pattern: Add an Audio Node

Derive from `AudioGraphNode`, implement `Render(nFrames)`, and connect to main mix:

```cpp
auto node = audioSubsystem->CreateAudioNode<MyAudioNode>();
audioSubsystem->ConnectToMainMixNode(node.get());
```

## Inferred Intent and Guardrails

- Prefer explicit graph wiring over hidden implicit coupling.
- Keep scene-layer boundaries clear (UI/gameplay/diagnostics separated).
- Keep app code framework-facing first; drop into core only for specialized behavior.
- Keep speculative features isolated behind concrete existing APIs.

## Related Chapters

- [Engine Architecture](./architecture/engine_architecture.md)
- [Core / Rendering](./subsystems/core_rendering.md)
- [Core / Audio](./subsystems/core_audio.md)
- [Framework / Scene](./subsystems/framework_scene.md)
