//
// Created by ozzadar on 2024-10-09.
//

#include "glfw_keys.h"

#ifdef OZZ_GLFW
#include "GLFW/glfw3.h"

namespace OZZ::platform::glfw {
    GLFWKey::GLFWKey(int glfwKeyCode) {
        switch (glfwKeyCode) {
            case GLFW_KEY_A: key = EKey::A; break;
            case GLFW_KEY_B: key = EKey::B; break;
            case GLFW_KEY_C: key = EKey::C; break;
            case GLFW_KEY_D: key = EKey::D; break;
            case GLFW_KEY_E: key = EKey::E; break;
            case GLFW_KEY_F: key = EKey::F; break;
            case GLFW_KEY_G: key = EKey::G; break;
            case GLFW_KEY_H: key = EKey::H; break;
            case GLFW_KEY_I: key = EKey::I; break;
            case GLFW_KEY_J: key = EKey::J; break;
            case GLFW_KEY_K: key = EKey::K; break;
            case GLFW_KEY_L: key = EKey::L; break;
            case GLFW_KEY_M: key = EKey::M; break;
            case GLFW_KEY_N: key = EKey::N; break;
            case GLFW_KEY_O: key = EKey::O; break;
            case GLFW_KEY_P: key = EKey::P; break;
            case GLFW_KEY_Q: key = EKey::Q; break;
            case GLFW_KEY_R: key = EKey::R; break;
            case GLFW_KEY_S: key = EKey::S; break;
            case GLFW_KEY_T: key = EKey::T; break;
            case GLFW_KEY_U: key = EKey::U; break;
            case GLFW_KEY_V: key = EKey::V; break;
            case GLFW_KEY_W: key = EKey::W; break;
            case GLFW_KEY_X: key = EKey::X; break;
            case GLFW_KEY_Y: key = EKey::Y; break;
            case GLFW_KEY_Z: key = EKey::Z; break;
            case GLFW_KEY_0: key = EKey::Num0; break;
            case GLFW_KEY_1: key = EKey::Num1; break;
            case GLFW_KEY_2: key = EKey::Num2; break;
            case GLFW_KEY_3: key = EKey::Num3; break;
            case GLFW_KEY_4: key = EKey::Num4; break;
            case GLFW_KEY_5: key = EKey::Num5; break;
            case GLFW_KEY_6: key = EKey::Num6; break;
            case GLFW_KEY_7: key = EKey::Num7; break;
            case GLFW_KEY_8: key = EKey::Num8; break;
            case GLFW_KEY_9: key = EKey::Num9; break;
            case GLFW_KEY_ESCAPE: key = EKey::Escape; break;
            case GLFW_KEY_LEFT_CONTROL: key = EKey::LControl; break;
            case GLFW_KEY_LEFT_SHIFT: key = EKey::LShift; break;
            case GLFW_KEY_LEFT_ALT: key = EKey::LAlt; break;
            case GLFW_KEY_LEFT_SUPER: key = EKey::LSystem; break;
            case GLFW_KEY_RIGHT_CONTROL: key = EKey::RControl; break;
            case GLFW_KEY_RIGHT_SHIFT: key = EKey::RShift; break;
            case GLFW_KEY_RIGHT_ALT: key = EKey::RAlt; break;
            case GLFW_KEY_RIGHT_SUPER: key = EKey::RSystem; break;
            case GLFW_KEY_MENU: key = EKey::Menu; break;
            case GLFW_KEY_LEFT_BRACKET: key = EKey::LBracket; break;
            case GLFW_KEY_RIGHT_BRACKET: key = EKey::RBracket; break;
            case GLFW_KEY_SEMICOLON: key = EKey::SemiColon; break;
            case GLFW_KEY_COMMA: key = EKey::Comma; break;
            case GLFW_KEY_PERIOD: key = EKey::Period; break;
            case GLFW_KEY_APOSTROPHE: key = EKey::Quote; break;
            case GLFW_KEY_SLASH: key = EKey::Slash; break;
            case GLFW_KEY_BACKSLASH: key = EKey::BackSlash; break;
            case GLFW_KEY_GRAVE_ACCENT: key = EKey::Tilde; break;
            case GLFW_KEY_EQUAL: key = EKey::Equal; break;
            case GLFW_KEY_MINUS: key = EKey::Dash; break;
            case GLFW_KEY_SPACE: key = EKey::Space; break;
            case GLFW_KEY_ENTER: key = EKey::Return; break;
            case GLFW_KEY_BACKSPACE: key = EKey::BackSpace; break;
            case GLFW_KEY_TAB: key = EKey::Tab; break;
            case GLFW_KEY_PAGE_UP: key = EKey::PageUp; break;
            case GLFW_KEY_PAGE_DOWN: key = EKey::PageDown; break;
            case GLFW_KEY_END: key = EKey::End; break;
            case GLFW_KEY_HOME: key = EKey::Home; break;
            case GLFW_KEY_INSERT: key = EKey::Insert; break;
            case GLFW_KEY_DELETE: key = EKey::Delete; break;
            case GLFW_KEY_KP_ADD: key = EKey::Add; break;
            case GLFW_KEY_KP_SUBTRACT: key = EKey::Subtract; break;
            case GLFW_KEY_KP_MULTIPLY: key = EKey::Multiply; break;
            case GLFW_KEY_KP_DIVIDE: key = EKey::Divide; break;
            case GLFW_KEY_LEFT: key = EKey::Left; break;
            case GLFW_KEY_RIGHT: key = EKey::Right; break;
            case GLFW_KEY_UP: key = EKey::Up; break;
            case GLFW_KEY_DOWN: key = EKey::Down; break;
            case GLFW_KEY_KP_0: key = EKey::Numpad0; break;
            case GLFW_KEY_KP_1: key = EKey::Numpad1; break;
            case GLFW_KEY_KP_2: key = EKey::Numpad2; break;
            case GLFW_KEY_KP_3: key = EKey::Numpad3; break;
            case GLFW_KEY_KP_4: key = EKey::Numpad4; break;
            case GLFW_KEY_KP_5: key = EKey::Numpad5; break;
            case GLFW_KEY_KP_6: key = EKey::Numpad6; break;
            case GLFW_KEY_KP_7: key = EKey::Numpad7; break;
            case GLFW_KEY_KP_8: key = EKey::Numpad8; break;
            case GLFW_KEY_KP_9: key = EKey::Numpad9; break;
            case GLFW_KEY_F1: key = EKey::F1; break;
            case GLFW_KEY_F2: key = EKey::F2; break;
            case GLFW_KEY_F3: key = EKey::F3; break;
            case GLFW_KEY_F4: key = EKey::F4; break;
            case GLFW_KEY_F5: key = EKey::F5; break;
            case GLFW_KEY_F6: key = EKey::F6; break;
            case GLFW_KEY_F7: key = EKey::F7; break;
            case GLFW_KEY_F8: key = EKey::F8; break;
            case GLFW_KEY_F9: key = EKey::F9; break;
            case GLFW_KEY_F10: key = EKey::F10; break;
            case GLFW_KEY_F11: key = EKey::F11; break;
            case GLFW_KEY_F12: key = EKey::F12; break;
            case GLFW_KEY_F13: key = EKey::F13; break;
            case GLFW_KEY_F14: key = EKey::F14; break;
            case GLFW_KEY_F15: key = EKey::F15; break;
            case GLFW_KEY_PAUSE: key = EKey::Pause; break;
            default: key = EKey::KeyCount; break; // Unknown key
        }
    }

    GLFWKeyState::GLFWKeyState(int glfwKeyState) {
        switch(glfwKeyState) {
            case GLFW_PRESS:
            case GLFW_REPEAT:
                state = EKeyState::KeyPressed;
                break;
            case GLFW_RELEASE: state = EKeyState::KeyReleased; break;
            default: state = EKeyState::Unknown; break; // Unknown key state
        }
    }
}

#endif