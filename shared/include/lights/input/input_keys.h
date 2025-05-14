//
// Created by ozzadar on 2024-10-09.
//

#pragma once

#include <iostream>
#include <array>
#include <unordered_map>
#include <variant>
#include <cstdint>

namespace OZZ {
    enum class EDeviceID : int8_t {
        Unknown = -3,
        Mouse = -2,
        Keyboard = -1,
        GamePad0,
        GamePad1,
        GamePad2,
        GamePad3,
        GamePad4,
        GamePad5,
        GamePad6,
        GamePad7,
        GamePad8,
    };
    enum class EKeyState : uint8_t {
        KeyReleased,
        KeyPressed,
        Unknown
    };

    enum class EKey : uint8_t {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Escape,
        LControl,
        LShift,
        LAlt,
        LSystem,
        RControl,
        RShift,
        RAlt,
        RSystem,
        Menu,
        LBracket,
        RBracket,
        SemiColon,
        Comma,
        Period,
        Quote,
        Slash,
        BackSlash,
        Tilde,
        Equal,
        Dash,
        Space,
        Return,
        BackSpace,
        Tab,
        PageUp,
        PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add,
        Subtract,
        Multiply,
        Divide,
        Left,
        Right,
        Up,
        Down,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        Pause,
        KeyCount
    };

    enum class EControllerButton : uint8_t {
        A = 0,
        B,
        X,
        Y,
        LeftShoulder,
        RightShoulder,
        Back,
        Start,
        Guide,
        LeftStick,
        RightStick,
        DPadUp,
        DPadRight,
        DPadDown,
        DPadLeft,
        LeftStickX,
        LeftStickY,
        RightStickX,
        RightStickY,
        LeftTrigger,
        RightTrigger,
        ButtonCount
    };

    enum class EMouseButton : uint8_t {
        Left,
        Right,
        Middle,
        Back,
        Forward,
        Extra0,
        Extra1,
        Extra2
    };

    constexpr int operator+(const EKey &key) {
        return static_cast<int>(key);
    }

    constexpr int operator+(const EControllerButton &button) {
        return static_cast<int>(button);
    }

    constexpr int operator+(const EKeyState &state) {
        return static_cast<int>(state);
    }

    constexpr int operator+(const EMouseButton &button) {
        return static_cast<int>(button);
    }

    constexpr int operator+(const EDeviceID &deviceId) {
        return static_cast<int>(deviceId);
    }

    using KeyStateArrayType = std::array<float, +EKey::KeyCount>;
    using ControllerStateArrayType = std::array<float, +EControllerButton::ButtonCount>;
    using ControllerStateMap = std::unordered_map<EDeviceID, ControllerStateArrayType>;


    struct InputKey {
        EDeviceID DeviceID = EDeviceID::Unknown;
        std::variant<EKey, EControllerButton, EMouseButton> Key = EKey::KeyCount;

        bool operator==(const InputKey &other) const {
            return DeviceID == other.DeviceID && Key == other.Key;
        }
    };
}

std::ostream &operator<<(std::ostream &out, const OZZ::EKeyState &state);

std::ostream &operator<<(std::ostream &out, const OZZ::EKey &key);