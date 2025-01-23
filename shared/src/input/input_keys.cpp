//
// Created by ozzadar on 2024-10-09.
//

#include <lights/input/input_keys.h>

std::ostream &operator<<(std::ostream &out, const OZZ::EKeyState &state) {
    using namespace OZZ;
    switch (state) {
        case EKeyState::KeyPressed:
            out << "Pressed";
            break;
        case EKeyState::KeyReleased:
            out << "Released";
            break;
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const OZZ::EKey &key) {
    using namespace OZZ;
    switch (key) {
        case EKey::A:
            out << "A";
            break;
        case EKey::B:
            out << "B";
            break;
        case EKey::C:
            out << "C";
            break;
        case EKey::D:
            out << "D";
            break;
        case EKey::E:
            out << "E";
            break;
        case EKey::F:
            out << "F";
            break;
        case EKey::G:
            out << "G";
            break;
        case EKey::H:
            out << "H";
            break;
        case EKey::I:
            out << "I";
            break;
        case EKey::J:
            out << "J";
            break;
        case EKey::K:
            out << "K";
            break;
        case EKey::L:
            out << "L";
            break;
        case EKey::M:
            out << "M";
            break;
        case EKey::N:
            out << "N";
            break;
        case EKey::O:
            out << "O";
            break;
        case EKey::P:
            out << "P";
            break;
        case EKey::Q:
            out << "Q";
            break;
        case EKey::R:
            out << "R";
            break;
        case EKey::S:
            out << "S";
            break;
        case EKey::T:
            out << "T";
            break;
        case EKey::U:
            out << "U";
            break;
        case EKey::V:
            out << "V";
            break;
        case EKey::W:
            out << "W";
            break;
        case EKey::X:
            out << "X";
            break;
        case EKey::Y:
            out << "Y";
            break;
        case EKey::Z:
            out << "Z";
            break;
        case EKey::Num0:
            out << "Num0";
            break;
        case EKey::Num1:
            out << "Num1";
            break;
        case EKey::Num2:
            out << "Num2";
            break;
        case EKey::Num3:
            out << "Num3";
            break;
        case EKey::Num4:
            out << "Num4";
            break;
        case EKey::Num5:
            out << "Num5";
            break;
        case EKey::Num6:
            out << "Num6";
            break;
        case EKey::Num7:
            out << "Num7";
            break;
        case EKey::Num8:
            out << "Num8";
            break;
        case EKey::Num9:
            out << "Num9";
            break;
        case EKey::Escape:
            out << "Escape";
            break;
        case EKey::LControl:
            out << "LControl";
            break;
        case EKey::LShift:
            out << "LShift";
            break;
        case EKey::LAlt:
            out << "LAlt";
            break;
        case EKey::LSystem:
            out << "LSystem";
            break;
        case EKey::RControl:
            out << "RControl";
            break;
        case EKey::RShift:
            out << "RShift";
            break;
        case EKey::RAlt:
            out << "RAlt";
            break;
        case EKey::RSystem:
            out << "RSystem";
            break;
        case EKey::Menu:
            out << "Menu";
            break;
        case EKey::LBracket:
            out << "LBracket";
            break;
        case EKey::RBracket:
            out << "RBracket";
            break;
        case EKey::SemiColon:
            out << "SemiColon";
            break;
        case EKey::Comma:
            out << "Comma";
            break;
        case EKey::Period:
            out << "Period";
            break;
        case EKey::Quote:
            out << "Quote";
            break;
        case EKey::Slash:
            out << "Slash";
            break;
        case EKey::BackSlash:
            out << "BackSlash";
            break;
        case EKey::Tilde:
            out << "Tilde";
            break;
        case EKey::Equal:
            out << "Equal";
            break;
        case EKey::Dash:
            out << "Dash";
            break;
        case EKey::Space:
            out << "Space";
            break;
        case EKey::Return:
            out << "Return";
            break;
        case EKey::BackSpace:
            out << "BackSpace";
            break;
        case EKey::Tab:
            out << "Tab";
            break;
        case EKey::PageUp:
            out << "PageUp";
            break;
        case EKey::PageDown:
            out << "PageDown";
            break;
        case EKey::End:
            out << "End";
            break;
        case EKey::Home:
            out << "Home";
            break;
        case EKey::Insert:
            out << "Insert";
            break;
        case EKey::Delete:
            out << "Delete";
            break;
        case EKey::Add:
            out << "Add";
            break;
        case EKey::Subtract:
            out << "Subtract";
            break;
        case EKey::Multiply:
            out << "Multiply";
            break;
        case EKey::Divide:
            out << "Divide";
            break;
        case EKey::Left:
            out << "Left";
            break;
        case EKey::Right:
            out << "Right";
            break;
        case EKey::Up:
            out << "Up";
            break;
        case EKey::Down:
            out << "Down";
            break;
        case EKey::Numpad0:
            out << "Numpad0";
            break;
        case EKey::Numpad1:
            out << "Numpad1";
            break;
        case EKey::Numpad2:
            out << "Numpad2";
            break;
        case EKey::Numpad3:
            out << "Numpad3";
            break;
        case EKey::Numpad4:
            out << "Numpad4";
            break;
        case EKey::Numpad5:
            out << "Numpad5";
            break;
        case EKey::Numpad6:
            out << "Numpad6";
            break;
        case EKey::Numpad7:
            out << "Numpad7";
            break;
        case EKey::Numpad8:
            out << "Numpad8";
            break;
        case EKey::Numpad9:
            out << "Numpad9";
            break;
        case EKey::F1:
            out << "F1";
            break;
        case EKey::F2:
            out << "F2";
            break;
        case EKey::F3:
            out << "F3";
            break;
        case EKey::F4:
            out << "F4";
            break;
        case EKey::F5:
            out << "F5";
            break;
        case EKey::F6:
            out << "F6";
            break;
        case EKey::F7:
            out << "F7";
            break;
        case EKey::F8:
            out << "F8";
            break;
        case EKey::F9:
            out << "F9";
            break;
        case EKey::F10:
            out << "F10";
            break;
        case EKey::F11:
            out << "F11";
            break;
        case EKey::F12:
            out << "F12";
            break;
        case EKey::F13:
            out << "F13";
            break;
        case EKey::F14:
            out << "F14";
            break;
        case EKey::F15:
            out << "F15";
            break;
        case EKey::Pause:
            out << "Pause";
            break;
        case EKey::KeyCount:
            out << "Unknown";
            break;
    }
    return out;
}
