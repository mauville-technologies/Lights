//
// Created by ozzadar on 2024-10-09.
//
#pragma once

#include <array>
#include <chrono>
#include <functional>
#include <glm/glm.hpp>
#include <lights/framework/input/input_keys.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace OZZ {
    using namespace std::chrono_literals;

    struct InputEvent {
        InputKey Key{};
        EKeyState State{};
    };

    struct InputChord {
        std::vector<InputKey> Keys;
        bool bIsSequence{false};
        bool bCanRepeat{false};
        std::chrono::duration<long long, std::milli> TimeBetweenKeys{1000};

        // These next values are not meant to be used directly
        std::vector<EKeyState> States;
        EKeyState CurrentState{EKeyState::KeyReleased};
        int CurrentKeyIndex{0};
        std::chrono::time_point<std::chrono::high_resolution_clock> LastKeyTime;

        /*
         * Receive an event from the input system
         * @param Key The key that was pressed
         * @param State The state of the key
         * @return True if there was a change in chord state
         */
        bool ReceiveEvent(InputKey Key, EKeyState State);
        void EnsureInitialized();

        bool bInitialized {false};
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

    using TextModeFunc = std::function<void(bool)>;
    using ClipboardGetFunc = std::function<std::string()>;
    using ClipboardSetFunc = std::function<void(const std::string&)>;

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
        void NotifyMouseMove(glm::vec2 inMousePosition);
        void NotifyMouseScroll(glm::vec2 inScrollDelta);

        void Tick(const KeyStateArrayType& inKeyStates,
                  const ControllerStateMap& inControllerStates,
                  const MouseButtonStateArrayType& inMouseButtonStates);

        void SetTextModeFunc(TextModeFunc&& InTextModeDelegate) { TextModeDelegate = std::move(InTextModeDelegate); }

        void SetTextMode(const bool bIsTextMode) const {
            if (TextModeDelegate) {
                TextModeDelegate(bIsTextMode);
            }
        }

        // The platform clipboard (UTF-8), wired up by the game to its window. Empty / no-op until then.
        void SetClipboardFuncs(ClipboardGetFunc&& InGet, ClipboardSetFunc&& InSet) {
            ClipboardGet = std::move(InGet);
            ClipboardSet = std::move(InSet);
        }

        [[nodiscard]] std::string GetClipboardText() const { return ClipboardGet ? ClipboardGet() : std::string{}; }

        void SetClipboardText(const std::string& text) const {
            if (ClipboardSet) {
                ClipboardSet(text);
            }
        }

        [[nodiscard]] const std::vector<InputMapping>& GetMappings() const { return mappings; }

        [[nodiscard]] const std::unordered_map<std::string, AxisMapping>& GetAxisMappings() const { return axisMappings; }

        [[nodiscard]] const std::vector<TextListenerMapping>& GetTextMappings() const { return textMappings; }

        [[nodiscard]] EKeyState GetKeyState(const InputKey& Key) const;
        [[nodiscard]] float GetAxisValue(const std::string& Action) const;
        [[nodiscard]] const glm::vec2& GetMousePosition() const;
        /** Wheel movement since the previous Tick (y > 0 = scrolled up / away from the user). */
        [[nodiscard]] const glm::vec2& GetScrollDelta() const { return scrollDelta; }

        ~InputSubsystem() = default;

    private:
        void Initialize();
        void Shutdown();

    private:
        TextModeFunc TextModeDelegate;
        ClipboardGetFunc ClipboardGet;
        ClipboardSetFunc ClipboardSet;

        std::unordered_map<std::string, AxisMapping> axisMappings{};
        std::vector<InputMapping> mappings{};
        std::vector<TextListenerMapping> textMappings{};

        glm::vec2 mousePosition{};
        glm::vec2 pendingScroll{};
        glm::vec2 scrollDelta{};
        KeyStateArrayType keyStates;
        ControllerStateMap controllerStates;
        MouseButtonStateArrayType mouseButtonStates;
        // A press that is already released by the next Tick (a quick click during a slow frame) still reads as down for
        // that one Tick, at the position it was pressed, so per-frame readers such as the UI do not miss it.
        std::array<bool, +EMouseButton::ButtonCount> mousePressedSinceTick{};
        std::array<glm::vec2, +EMouseButton::ButtonCount> mousePressPosition{};
        glm::vec2 latestMousePosition{};
    };
} // namespace OZZ
