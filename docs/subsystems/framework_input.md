# Framework Subsystem: Input

Path: `engine/include/lights/framework/input/*`

## What It Contains

Framework input provides a device-agnostic mapping layer:
- key/device enums and state containers (`input_keys.h`);
- action/chord/axis/text mapping runtime (`InputSubsystem`).

## Mapping Model (concrete)

- `InputMapping`: named action + one or more `InputChord`s.
- `AxisMapping`: named axis + weighted key/button sources.
- `TextListenerMapping`: named text callback listeners.

`InputChord` supports:
- multi-key chord matching;
- optional sequence mode;
- repeat policy and sequence timing window.

## Runtime Behavior

`InputSubsystem::Tick(...)`:
- updates cached keyboard/mouse/controller states;
- synthesizes digital button events from analog stick/trigger transitions;
- computes axis values with deadzone and clamping;
- dispatches action callbacks when chord states change.

## Device Abstraction

`InputKey` combines:
- `EDeviceID` (Keyboard, Mouse, GamePad0..GamePad8, etc.)
- variant key type (`EKey`, `EControllerButton`, `EMouseButton`)

This enables mixed-device chord definitions in one action mapping.

## Inferred Design Intent

- unify input behavior across keyboard/mouse/gamepad;
- keep application code action-oriented instead of device-code oriented;
- enable flexible rebinding-style workflows through mapping APIs.

## Speculative Direction (labeled)

Potential enhancements:
- configurable deadzones and response curves;
- richer text input/editing modes;
- input context stacks (menu/gameplay/editor) layered above current mapping model.
