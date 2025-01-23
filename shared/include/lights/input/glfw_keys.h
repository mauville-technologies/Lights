//
// Created by ozzadar on 2024-10-09.
//

#pragma once

#include "input_keys.h"

namespace OZZ {
    class GLFWKey {
    public:
        GLFWKey() : key(EKey::KeyCount) {};
        GLFWKey(int glfwKeyCode);

        operator EKey() const { return key; }
    private:
        EKey key;
    };

    class GLFWKeyState {
    public:
        GLFWKeyState() : state(EKeyState::KeyReleased) {};
        explicit GLFWKeyState(int glfwKeyState);

        operator EKeyState() const { return state; }
    private:
        EKeyState state;
    };
}
