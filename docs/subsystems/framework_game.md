# Framework Subsystem: Game

Path: `engine/include/lights/framework/game/*`

## What It Contains

Framework game provides `LightsGame<SceneType>`, the primary application bootstrap and runtime loop orchestrator.

## Configuration Model (concrete)

`GameParameters` supports TOML-driven settings:
- engine: target FPS
- window: mode + size
- audio: sample rate + channels

`LightsGame` consumes configuration through `Configuration<GameParameters>`.

## Lifecycle

`Run()` initializes in this order:
1. input
2. window/platform callbacks
3. audio subsystem
4. GL viewport state
5. renderer
6. scene

Then loops:
- scene end checks;
- resource manager tick;
- scene tick;
- conditional render at configured rate;
- event polling + input tick;
- frame sleep control.

## Callback Wiring (concrete)

Window callbacks are bridged into framework systems:
- close -> stop run loop
- resize -> update viewport + scene resize
- key/text/mouse events -> input subsystem notifications

## Inferred Design Intent

- provide a stable "engine shell" for applications;
- keep app entrypoint small (`LightsGame<Scene>::Run()` pattern);
- centralize subsystem startup/shutdown order.

## Speculative Direction (labeled)

Likely future additions:
- richer frame scheduling/tick policy controls;
- explicit fixed-update/render decoupling knobs;
- additional lifecycle hooks around init/shutdown.
