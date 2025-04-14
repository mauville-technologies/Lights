//
// Created by ozzadar on 2024-10-09.
//
#pragma once

#include <lights/input/input_keys.h>
#include <string>
#include <functional>
#include <vector>
#include <chrono>
#include <variant>

namespace OZZ {
    using namespace std::chrono_literals;

    struct InputEvent {
        InputKey Key {};
        EKeyState State {};
    };

    struct InputChord {
        std::vector<InputKey> Keys;
        bool bIsSequence { false };
        bool bCanRepeat { false };
        std::chrono::duration<long long, std::milli> TimeBetweenKeys { 1000 };

        // These next values are not meant to be used directly
        std::vector<EKeyState> States;
        EKeyState CurrentState { EKeyState::KeyReleased };
        int CurrentKeyIndex { 0 };
        std::chrono::time_point<std::chrono::high_resolution_clock> LastKeyTime;


        /*
         * Receive an event from the input system
         * @param Key The key that was pressed
         * @param State The state of the key
         * @return True if there was a change in chord state
         */
        bool ReceiveEvent(InputKey Key, EKeyState State);
        void EnsureInitialized();

        bool bInitialized { false };
    };

    struct ActionCallbacks {
        std::function<void()> OnPressed;
        std::function<void()> OnReleased;
    };

    struct InputMapping {
        std::string Action;
        std::vector<InputChord> Chords;

        ActionCallbacks Callbacks;
    };

    struct AxisMapping {
        std::string Action;
        std::vector<std::pair<InputKey, float>> Keys;
        float Value;
    };

    struct TextListenerMapping {
        std::string Name;
        std::function<void(char)> Callback;
    };

    class InputSubsystem {
    public:
        InputSubsystem();
        void RegisterInputMapping(InputMapping&& Mapping);
        void UnregisterInputMapping(const std::string& Action);

        void RegisterAxisMapping(AxisMapping&& Mapping);
        void UnregisterAxisMapping(const std::string& Action);

        void RegisterTextListener(TextListenerMapping&& Mapping);
        void UnregisterTextListener(const std::string& Name);

        void NotifyInputEvent(const InputEvent& Event);
        void NotifyTextEvent(char character);

        void Tick(const KeyStateArrayType &keyStates, const ControllerStateMap& controllerStates);

        [[nodiscard]] float GetAxisValue(const std::string& Action) const;

        ~InputSubsystem() = default;
    private:

        void Initialize();
        void Shutdown();


    private:
        std::vector<AxisMapping> AxisMappings {};
        std::vector<InputMapping> Mappings {};
        std::vector<TextListenerMapping> TextMappings {};
    };
} // OZZ
