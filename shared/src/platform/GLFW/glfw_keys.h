//
// Created by ozzadar on 2024-10-09.
//

#pragma once

#ifdef OZZ_GLFW
#include <lights/input/input_keys.h>

namespace OZZ::platform::glfw {
    class GLFWKey {
    public:
        GLFWKey() : key(EKey::KeyCount) {};
        GLFWKey(int glfwKeyCode);

        operator EKey() const { return key; }
        operator int() const { return +key; }
    private:
        EKey key;
    };

    class GLFWKeyState {
    public:
        GLFWKeyState() : state(EKeyState::KeyReleased) {};
        explicit GLFWKeyState(int glfwKeyState);

        operator EKeyState() const { return state; }
        operator float() const {
            switch (state) {
                case EKeyState::KeyPressed:
                    return 1.f;
                default:
                    return 0;
            }
        }
    private:
        EKeyState state;
    };
}

#endif