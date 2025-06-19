//
// Created by ozzadar on 2025-06-19.
//

#include "sdl_keys.h"


#ifdef OZZ_SDL3

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

namespace OZZ::platform::sdl3 {
    SDLKey::SDLKey(int sdlKeyCode) {
        switch (sdlKeyCode) {
            case SDLK_A: key = EKey::A; break;
            case SDLK_B: key = EKey::B; break;
            case SDLK_C: key = EKey::C; break;
            case SDLK_D: key = EKey::D; break;
            case SDLK_E: key = EKey::E; break;
            case SDLK_F: key = EKey::F; break;
            case SDLK_G: key = EKey::G; break;
            case SDLK_H: key = EKey::H; break;
            case SDLK_I: key = EKey::I; break;
            case SDLK_J: key = EKey::J; break;
            case SDLK_K: key = EKey::K; break;
            case SDLK_L: key = EKey::L; break;
            case SDLK_M: key = EKey::M; break;
            case SDLK_N: key = EKey::N; break;
            case SDLK_O: key = EKey::O; break;
            case SDLK_P: key = EKey::P; break;
            case SDLK_Q: key = EKey::Q; break;
            case SDLK_R: key = EKey::R; break;
            case SDLK_S: key = EKey::S; break;
            case SDLK_T: key = EKey::T; break;
            case SDLK_U: key = EKey::U; break;
            case SDLK_V: key = EKey::V; break;
            case SDLK_W: key = EKey::W; break;
            case SDLK_X: key = EKey::X; break;
            case SDLK_Y: key = EKey::Y; break;
            case SDLK_Z: key = EKey::Z; break;
            case SDLK_0: key = EKey::Num0; break;
            case SDLK_1: key = EKey::Num1; break;
            case SDLK_2: key = EKey::Num2; break;
            case SDLK_3: key = EKey::Num3; break;
            case SDLK_4: key = EKey::Num4; break;
            case SDLK_5: key = EKey::Num5; break;
            case SDLK_6: key = EKey::Num6; break;
            case SDLK_7: key = EKey::Num7; break;
            case SDLK_8: key = EKey::Num8; break;
            case SDLK_9: key = EKey::Num9; break;
            case SDLK_ESCAPE: key = EKey::Escape; break;
            case SDLK_LCTRL: key = EKey::LControl; break;
            case SDLK_LSHIFT: key = EKey::LShift; break;
            case SDLK_LALT: key = EKey::LAlt; break;
            case SDLK_LGUI: key = EKey::LSystem; break;
            case SDLK_RCTRL: key = EKey::RControl; break;
            case SDLK_RSHIFT: key = EKey::RShift; break;
            case SDLK_RALT: key = EKey::RAlt; break;
            case SDLK_RGUI: key = EKey::RSystem; break;
            case SDLK_MENU: key = EKey::Menu; break;
            case SDLK_LEFTBRACKET: key = EKey::LBracket; break;
            case SDLK_RIGHTBRACKET: key = EKey::RBracket; break;
            case SDLK_SEMICOLON: key = EKey::SemiColon; break;
            case SDLK_COMMA: key = EKey::Comma; break;
            case SDLK_PERIOD: key = EKey::Period; break;
            case SDLK_APOSTROPHE: key = EKey::Quote; break;
            case SDLK_SLASH: key = EKey::Slash; break;
            case SDLK_BACKSLASH: key = EKey::BackSlash; break;
            case SDLK_TILDE: key = EKey::Tilde; break;
            case SDLK_EQUALS: key = EKey::Equal; break;
            case SDLK_MINUS: key = EKey::Dash; break;
            case SDLK_SPACE: key = EKey::Space; break;
            case SDLK_RETURN: key = EKey::Return; break;
            case SDLK_BACKSPACE: key = EKey::BackSpace; break;
            case SDLK_TAB: key = EKey::Tab; break;
            case SDLK_PAGEUP: key = EKey::PageUp; break;
            case SDLK_PAGEDOWN: key = EKey::PageDown; break;
            case SDLK_HOME: key = EKey::Home; break;
            case SDLK_END: key = EKey::End; break;
            case SDLK_INSERT: key = EKey::Insert; break;
            case SDLK_DELETE: key = EKey::Delete; break;
            case SDLK_KP_PLUS: key = EKey::Add; break;
            case SDLK_KP_MINUS: key = EKey::Subtract; break;
            case SDLK_KP_MULTIPLY: key = EKey::Multiply; break;
            case SDLK_KP_DIVIDE: key = EKey::Divide; break;
            case SDLK_LEFT: key = EKey::Left; break;
            case SDLK_RIGHT: key = EKey::Right; break;
            case SDLK_UP: key = EKey::Up; break;
            case SDLK_DOWN: key = EKey::Down; break;
            case SDLK_KP_0: key = EKey::Numpad0; break;
            case SDLK_KP_1: key = EKey::Numpad1; break;
            case SDLK_KP_2: key = EKey::Numpad2; break;
            case SDLK_KP_3: key = EKey::Numpad3; break;
            case SDLK_KP_4: key = EKey::Numpad4; break;
            case SDLK_KP_5: key = EKey::Numpad5; break;
            case SDLK_KP_6: key = EKey::Numpad6; break;
            case SDLK_KP_7: key = EKey::Numpad7; break;
            case SDLK_KP_8: key = EKey::Numpad8; break;
            case SDLK_KP_9: key = EKey::Numpad9; break;
            case SDLK_F1: key = EKey::F1; break;
            case SDLK_F2: key = EKey::F2; break;
            case SDLK_F3: key = EKey::F3; break;
            case SDLK_F4: key = EKey::F4; break;
            case SDLK_F5: key = EKey::F5; break;
            case SDLK_F6: key = EKey::F6; break;
            case SDLK_F7: key = EKey::F7; break;
            case SDLK_F8: key = EKey::F8; break;
            case SDLK_F9: key = EKey::F9; break;
            case SDLK_F10: key = EKey::F10; break;
            case SDLK_F11: key = EKey::F11; break;
            case SDLK_F12: key = EKey::F12; break;
            case SDLK_F13: key = EKey::F13; break;
            case SDLK_F14: key = EKey::F14; break;
            case SDLK_F15: key = EKey::F15; break;
            case SDLK_PAUSE: key = EKey::Pause; break;
            default: key = EKey::KeyCount; break;



        }
    }

    SDLMouseButton::SDLMouseButton(int sdlMouseButton) {
        switch (sdlMouseButton) {
            case SDL_BUTTON_LEFT: button = EMouseButton::Left; break;
            case SDL_BUTTON_RIGHT: button = EMouseButton::Right; break;
            case SDL_BUTTON_MIDDLE: button = EMouseButton::Middle; break;
            case SDL_BUTTON_X1: button = EMouseButton::Back; break;
            case SDL_BUTTON_X2: button = EMouseButton::Forward; break;
            default: button = EMouseButton::ButtonCount; break; // Unknown mouse button
        }
    }

    SDLKeyState::SDLKeyState(int sdlKeyState) {
        switch (sdlKeyState) {
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_KEY_DOWN: {
                state = EKeyState::KeyPressed;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_KEY_UP: {
                state = EKeyState::KeyReleased;
                break;
            }
            default: {
                state = EKeyState::Unknown; // Unknown key state
                break;
            }
        }
    }
}

#endif