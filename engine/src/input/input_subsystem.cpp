//
// Created by ozzadar on 2024-10-09.
//

#include "lights/input/input_subsystem.h"

#include "lights/util/enums.h"

#include <cassert>

#include <algorithm>
#include <spdlog/spdlog.h>

namespace OZZ {
    InputSubsystem::InputSubsystem() {}

    void InputSubsystem::Initialize() {}

    void InputSubsystem::Shutdown() {}

    void InputSubsystem::NotifyInputEvent(const InputEvent& Event) {
        // Notify all mappings
        for (auto& Mapping : mappings) {
            // loops through all chords, if one of them is triggered, call the callback
            auto& chords = Mapping.Chords;
            for (auto& Chord : chords) {
                if (Chord.ReceiveEvent(Event.Key, Event.State)) {
                    switch (Chord.CurrentState) {
                        case EKeyState::KeyPressed:
                            if (Mapping.Callbacks.OnPressed)
                                Mapping.Callbacks.OnPressed();
                            break;
                        case EKeyState::KeyReleased:
                            if (Mapping.Callbacks.OnReleased)
                                Mapping.Callbacks.OnReleased();
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }

    void InputSubsystem::NotifyTextEvent(char character) {
        // Notify all text mappings
        for (auto& [Name, Callback] : textMappings) {
            if (Callback) {
                Callback(character);
            }
        }
    }

    void InputSubsystem::NotifyMouseMove(glm::vec2 inMousePosition) {
        mousePosition = inMousePosition;
    }

    void InputSubsystem::Tick(const KeyStateArrayType& inKeyStates,
                              const ControllerStateMap& inControllerStates,
                              const MouseButtonStateArrayType& inMouseButtonStates) {

        // TODO: We'll want this to be more efficient in the future
        const auto previousControllerStates = controllerStates;
        keyStates = inKeyStates;
        controllerStates = inControllerStates;
        mouseButtonStates = inMouseButtonStates;

        const auto NotifyAxisToButtonEvent = [this](const ControllerStateArrayType& PreviousState,
                                                    const ControllerStateArrayType& CurrentState,
                                                    EDeviceID DeviceID,
                                                    const EControllerButton AxisButton,
                                                    EControllerButton PositiveDigital,
                                                    EControllerButton NegativeDigital) {
            const auto prevAxisState = PreviousState[to_index(AxisButton)];
            const auto currentAxisState = CurrentState[to_index(AxisButton)];

            // hard coded dead zone
            auto oldDirection = 0.f;
            auto newDirection = 0.f;

            const auto bWasPressed = prevAxisState > 0.1f || prevAxisState < -0.1f;
            if (bWasPressed) {
                oldDirection = prevAxisState > 0.f ? 1.f : -1.f;
            }
            const auto bIsPressed = currentAxisState > 0.1f || currentAxisState < -0.1f;
            if (bIsPressed) {
                newDirection = currentAxisState > 0.f ? 1.f : -1.f;
            }

            if (oldDirection != newDirection) {
                // if old direction is left
                if (oldDirection == -1) {
                    if (newDirection == 0.f) {
                        // Release LeftStickLeft
                        auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = NegativeDigital},
                            .State = EKeyState::KeyReleased,
                        };
                        NotifyInputEvent(newEvent);
                    } else if (newDirection == 1.f) {
                        // Release LeftStickLeft
                        auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = NegativeDigital},
                            .State = EKeyState::KeyReleased,
                        };
                        NotifyInputEvent(newEvent);
                        // Press LeftStickRight
                        // Release LeftStickLeft
                        newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = PositiveDigital},
                            .State = EKeyState::KeyPressed,
                        };
                        NotifyInputEvent(newEvent);
                    }
                } else if (oldDirection == 0.f) {
                    if (newDirection == -1.f) {
                        const auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = NegativeDigital},
                            .State = EKeyState::KeyPressed,
                        };
                        NotifyInputEvent(newEvent);
                    } else if (newDirection == 1.f) {
                        const auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = PositiveDigital},
                            .State = EKeyState::KeyPressed,
                        };
                        NotifyInputEvent(newEvent);
                    }
                } else {
                    if (newDirection == 0.f) {
                        // Release LeftStickRight
                        auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = PositiveDigital},
                            .State = EKeyState::KeyReleased,
                        };
                        NotifyInputEvent(newEvent);
                    } else if (newDirection == -1.f) {
                        // Release LeftStickRight
                        auto newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = PositiveDigital},
                            .State = EKeyState::KeyReleased,
                        };
                        NotifyInputEvent(newEvent);
                        // Press LeftStickLeft
                        newEvent = InputEvent{
                            .Key = {.DeviceID = DeviceID, .Key = NegativeDigital},
                            .State = EKeyState::KeyPressed,
                        };
                        NotifyInputEvent(newEvent);
                    }
                }
            }
        };
        // generate synthetic input events for axis to be able to use as button inputs
        for (const auto [gamepad, prevState] : previousControllerStates) {
            const auto newState = controllerStates[gamepad];
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::LeftStickX,
                                    EControllerButton::LeftStickXRight,
                                    EControllerButton::LeftStickXLeft);
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::LeftStickY,
                                    EControllerButton::LeftStickYDown,
                                    EControllerButton::LeftStickYUp);
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::RightStickX,
                                    EControllerButton::RightStickXRight,
                                    EControllerButton::RightStickXLeft);
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::RightStickY,
                                    EControllerButton::RightStickYDown,
                                    EControllerButton::RightStickYUp);
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::RightTrigger,
                                    EControllerButton::RightTrigger,
                                    EControllerButton::RightTrigger);
            NotifyAxisToButtonEvent(prevState,
                                    newState,
                                    gamepad,
                                    EControllerButton::LeftTrigger,
                                    EControllerButton::LeftTrigger,
                                    EControllerButton::LeftTrigger);
        }

        for (auto& Mapping : axisMappings) {
            Mapping.Value = 0.f;

            for (auto& [eKey, Weight] : Mapping.Keys) {
                if (Mapping.Value != 0.f) {
                    break;
                }

                if (eKey.DeviceID == EDeviceID::Keyboard) {
                    Mapping.Value = std::clamp(+inKeyStates[+std::get<EKey>(eKey.Key)] * Weight, -1.f, 1.f);
                } else {
                    // this is a controller mapping
                    if (inControllerStates.contains(eKey.DeviceID)) {
                        const auto key = std::get<EControllerButton>(eKey.Key);
                        auto value = inControllerStates.at(eKey.DeviceID)[+key];

                        // TODO: Current deadzone is hardcoded. It might make sense to make
                        // this customizable
                        value = value < 0.1f && value > -0.1f ? 0.f : value;

                        Mapping.Value = std::clamp(value * Weight, -1.f, 1.f);
                    }
                }
            }
        }
    }

    EKeyState InputSubsystem::GetKeyState(const InputKey& Key) const {
        switch (Key.DeviceID) {
            case EDeviceID::Keyboard: {
                return keyStates[+std::get<EKey>(Key.Key)];
            }
            case EDeviceID::Mouse: {
                return mouseButtonStates[+std::get<EMouseButton>(Key.Key)];
            }
            default: {
                if (controllerStates.contains(Key.DeviceID)) {
                    const auto button = std::get<EControllerButton>(Key.Key);
                    return static_cast<EKeyState>(static_cast<int>(controllerStates.at(Key.DeviceID)[+button]));
                }
                break;
            }
        }
    }

    float InputSubsystem::GetAxisValue(const std::string& Action) const {
        auto Mapping = std::ranges::find_if(axisMappings, [&Action](const AxisMapping& Mapping) {
            return Mapping.Action == Action;
        });

        if (Mapping != axisMappings.end()) {
            return Mapping->Value;
        }
        return 0.f;
    }

    const glm::vec2& InputSubsystem::GetMousePosition() const {
        return mousePosition;
    }

    void InputSubsystem::RegisterInputMapping(InputMapping&& Mapping) {
        std::string Action = Mapping.Action;
        bool bFound = false;
        for (auto& ExistingMapping : mappings) {
            if (ExistingMapping.Action == Action) {
                ExistingMapping = Mapping;
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            mappings.push_back(Mapping);
        }
    }

    void InputSubsystem::UnregisterInputMapping(const std::string& Action) {
        std::erase_if(mappings, [&Action](const InputMapping& Mapping) {
            return Mapping.Action == Action;
        });
    }

    void InputSubsystem::RegisterAxisMapping(AxisMapping&& Mapping) {
        const std::string& action = Mapping.Action;
        bool bFound = false;
        for (auto& ExistingMapping : axisMappings) {
            if (ExistingMapping.Action == action) {
                ExistingMapping = Mapping;
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            axisMappings.push_back(Mapping);
        }
    }

    void InputSubsystem::UnregisterAxisMapping(const std::string& Action) {
        std::erase_if(axisMappings, [&Action](const AxisMapping& Mapping) {
            return Mapping.Action == Action;
        });
    }

    void InputSubsystem::RegisterTextListener(TextListenerMapping&& Mapping) {
        const std::string& name = Mapping.Name;
        bool bFound = false;
        for (auto& ExistingMapping : textMappings) {
            if (ExistingMapping.Name == name) {
                ExistingMapping = Mapping;
                bFound = true;
                break;
            }
        }
        if (!bFound) {
            textMappings.push_back(Mapping);
        }
    }

    void InputSubsystem::UnregisterTextListener(const std::string& Name) {
        std::erase_if(textMappings, [&Name](const TextListenerMapping& Mapping) {
            return Mapping.Name == Name;
        });
    }

    bool InputChord::ReceiveEvent(InputKey Key, EKeyState State) {
        EnsureInitialized();

        bool bChangedState = false;
        for (size_t i = 0; i < Keys.size(); i++) {
            if (Keys[i] == Key) {
                States[i] = State;
                bChangedState = true;
            }
        }

        if (!bIsSequence) {
            // Calculate new state
            bool bAllPressed = true;
            for (const auto& KeyState : States) {
                if (KeyState == EKeyState::KeyReleased) {
                    bAllPressed = false;
                    break;
                }
            }

            EKeyState NewState{bAllPressed ? EKeyState::KeyPressed : EKeyState::KeyReleased};

            if (NewState != CurrentState || bCanRepeat) {
                CurrentState = NewState;
                return true;
            }
        } else {
            auto CurrentTime = std::chrono::high_resolution_clock::now();
            bool bProgressSequence = (CurrentKeyIndex == 0 || CurrentTime - LastKeyTime < TimeBetweenKeys) &&
                                     bChangedState && State == EKeyState::KeyPressed && Keys[CurrentKeyIndex] == Key;

            // if you screw up, you want to be able to start over at any point.
            if (!bProgressSequence && State == EKeyState::KeyPressed && Key == Keys[0]) {
                CurrentKeyIndex = 0;
                bProgressSequence = true;
            }

            if (bProgressSequence) {
                if (CurrentKeyIndex == Keys.size() - 1) {
                    CurrentState = EKeyState::KeyPressed;
                    CurrentKeyIndex = 0;
                    LastKeyTime = CurrentTime;
                    return true;
                }

                CurrentKeyIndex++;
                LastKeyTime = CurrentTime;
            } else if (State == EKeyState::KeyPressed) {
                CurrentKeyIndex = 0;
            }

            CurrentState = EKeyState::KeyReleased;
        }

        return false;
    }

    void InputChord::EnsureInitialized() {
        if (!bInitialized) {
            States.resize(Keys.size());
            for (auto& State : States) {
                State = EKeyState::KeyReleased;
            }
            LastKeyTime = std::chrono::high_resolution_clock::now();
            bInitialized = true;
        }
    }
} // namespace OZZ
