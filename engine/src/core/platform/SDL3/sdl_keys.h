//
// Created by ozzadar on 2025-06-19.
//

#pragma once

#ifdef OZZ_SDL3
#include <SDL3/SDL_gamepad.h>
#include <lights/framework/input/input_keys.h>

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

    class SDLGamepadButton {
    private:
        SDLGamepadButton() : button(EControllerButton::ButtonCount) {};

    public:
        SDLGamepadButton(SDL_GamepadButton sdlGamepadButton);
        SDLGamepadButton(SDL_GamepadAxis sdlAxis);

        operator EControllerButton() const { return button; }

        operator int() const { return +button; }

    private:
        EControllerButton button;
    };

    class SDLKeyState {
    public:
        SDLKeyState() : state(EKeyState::KeyReleased) {};
        explicit SDLKeyState(int sdlKeyState);

        operator EKeyState() const { return state; }

        operator int() const {
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
} // namespace OZZ::platform::sdl3

#endif