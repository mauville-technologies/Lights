//
// Created by ozzadar on 2025-06-19.
//

#pragma once

#ifdef OZZ_SDL3
#include <lights/input/input_keys.h>

namespace OZZ::platform::sdl3 {
    class SDLKey {
    private:
        // delete this constructor
        SDLKey() : key(EKey::KeyCount) {};
    public:
        SDLKey(int sdlKeyCode);

        operator EKey() const { return key; }
        operator int() const { return +key; }
    private:
        EKey key;
    };

    class SDLMouseButton {
    private:
        SDLMouseButton() : button(EMouseButton::ButtonCount) {};
    public:
        SDLMouseButton(int sdlMouseButton);

        operator EMouseButton() const { return button; }
        operator int() const { return +button; }

    private:
        EMouseButton button;
    };

    class SDLKeyState {
    public:
        SDLKeyState() : state(EKeyState::KeyReleased) {};
        explicit SDLKeyState(int sdlKeyState);

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