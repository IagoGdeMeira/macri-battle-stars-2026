#include "InputMapper/InputMapper.h"

#include <stdexcept>

InputAction InputMapper::stringToAction(const std::string& actionStr)
{
    if (actionStr == "MoveUp")      return InputAction::MoveUp;
    if (actionStr == "MoveDown")    return InputAction::MoveDown;
    if (actionStr == "MoveLeft")    return InputAction::MoveLeft;
    if (actionStr == "MoveRight")   return InputAction::MoveRight;
    if (actionStr == "Jump")        return InputAction::Jump;
    if (actionStr == "Punch")       return InputAction::Punch;
    if (actionStr == "Kick")        return InputAction::Kick;
    if (actionStr == "Defend")      return InputAction::Defend;
    if (actionStr == "Crouch")      return InputAction::Crouch;
    
    return InputAction::None;
}

std::string InputMapper::actionToString(InputAction action)
{
    switch (action)
    {
        case InputAction::MoveUp:     return "MoveUp";
        case InputAction::MoveDown:   return "MoveDown";
        case InputAction::MoveLeft:   return "MoveLeft";
        case InputAction::MoveRight:  return "MoveRight";
        case InputAction::Jump:       return "Jump";
        case InputAction::Punch:      return "Punch";
        case InputAction::Kick:       return "Kick";
        case InputAction::Defend:     return "Defend";
        case InputAction::Crouch:     return "Crouch";
        
        default: return "None";
    }
}

KeyCode InputMapper::stringToKey(const std::string& keyStr)
{
    if (keyStr == "A")          return KeyCode::A;
    if (keyStr == "B")          return KeyCode::B;
    if (keyStr == "C")          return KeyCode::C;
    if (keyStr == "D")          return KeyCode::D;
    if (keyStr == "E")          return KeyCode::E;
    if (keyStr == "F")          return KeyCode::F;
    if (keyStr == "G")          return KeyCode::G;
    if (keyStr == "H")          return KeyCode::H;
    if (keyStr == "I")          return KeyCode::I;
    if (keyStr == "J")          return KeyCode::J;
    if (keyStr == "K")          return KeyCode::K;
    if (keyStr == "L")          return KeyCode::L;
    if (keyStr == "M")          return KeyCode::M;
    if (keyStr == "N")          return KeyCode::N;
    if (keyStr == "O")          return KeyCode::O;
    if (keyStr == "P")          return KeyCode::P;
    if (keyStr == "Q")          return KeyCode::Q;
    if (keyStr == "R")          return KeyCode::R;
    if (keyStr == "S")          return KeyCode::S;
    if (keyStr == "T")          return KeyCode::T;
    if (keyStr == "U")          return KeyCode::U;
    if (keyStr == "V")          return KeyCode::V;
    if (keyStr == "W")          return KeyCode::W;
    if (keyStr == "X")          return KeyCode::X;
    if (keyStr == "Y")          return KeyCode::Y;
    if (keyStr == "Z")          return KeyCode::Z;
    if (keyStr == "Num0")       return KeyCode::Num0;
    if (keyStr == "Num1")       return KeyCode::Num1;
    if (keyStr == "Num2")       return KeyCode::Num2;
    if (keyStr == "Num3")       return KeyCode::Num3;
    if (keyStr == "Num4")       return KeyCode::Num4;
    if (keyStr == "Num5")       return KeyCode::Num5;
    if (keyStr == "Num6")       return KeyCode::Num6;
    if (keyStr == "Num7")       return KeyCode::Num7;
    if (keyStr == "Num8")       return KeyCode::Num8;
    if (keyStr == "Num9")       return KeyCode::Num9;
    if (keyStr == "Escape")     return KeyCode::Escape;
    if (keyStr == "LControl")   return KeyCode::LControl;
    if (keyStr == "LShift")     return KeyCode::LShift;
    if (keyStr == "LAlt")       return KeyCode::LAlt;
    if (keyStr == "LSystem")    return KeyCode::LSystem;
    if (keyStr == "RControl")   return KeyCode::RControl;
    if (keyStr == "RShift")     return KeyCode::RShift;
    if (keyStr == "RAlt")       return KeyCode::RAlt;
    if (keyStr == "RSystem")    return KeyCode::RSystem;
    if (keyStr == "Menu")       return KeyCode::Menu;
    if (keyStr == "LBracket")   return KeyCode::LBracket;
    if (keyStr == "RBracket")   return KeyCode::RBracket;
    if (keyStr == "Semicolon")  return KeyCode::Semicolon;
    if (keyStr == "Comma")      return KeyCode::Comma;
    if (keyStr == "Period")     return KeyCode::Period;
    if (keyStr == "Quote")      return KeyCode::Quote;
    if (keyStr == "Slash")      return KeyCode::Slash;
    if (keyStr == "Backslash")  return KeyCode::Backslash;
    if (keyStr == "Tilde")      return KeyCode::Tilde;
    if (keyStr == "Equal")      return KeyCode::Equal;
    if (keyStr == "Hyphen")     return KeyCode::Hyphen;
    if (keyStr == "Space")      return KeyCode::Space;
    if (keyStr == "Enter")      return KeyCode::Enter;
    if (keyStr == "Backspace")  return KeyCode::Backspace;
    if (keyStr == "Tab")        return KeyCode::Tab;
    if (keyStr == "PageUp")     return KeyCode::PageUp;
    if (keyStr == "PageDown")   return KeyCode::PageDown;
    if (keyStr == "End")        return KeyCode::End;
    if (keyStr == "Home")       return KeyCode::Home;
    if (keyStr == "Insert")     return KeyCode::Insert;
    if (keyStr == "Delete")     return KeyCode::Delete;
    if (keyStr == "Add")        return KeyCode::Add;
    if (keyStr == "Subtract")   return KeyCode::Subtract;
    if (keyStr == "Multiply")   return KeyCode::Multiply;
    if (keyStr == "Divide")     return KeyCode::Divide;
    if (keyStr == "Left")       return KeyCode::Left;
    if (keyStr == "Right")      return KeyCode::Right;
    if (keyStr == "Up")         return KeyCode::Up;
    if (keyStr == "Down")       return KeyCode::Down;
    if (keyStr == "Numpad0")    return KeyCode::Numpad0;
    if (keyStr == "Numpad1")    return KeyCode::Numpad1;
    if (keyStr == "Numpad2")    return KeyCode::Numpad2;
    if (keyStr == "Numpad3")    return KeyCode::Numpad3;
    if (keyStr == "Numpad4")    return KeyCode::Numpad4;
    if (keyStr == "Numpad5")    return KeyCode::Numpad5;
    if (keyStr == "Numpad6")    return KeyCode::Numpad6;
    if (keyStr == "Numpad7")    return KeyCode::Numpad7;
    if (keyStr == "Numpad8")    return KeyCode::Numpad8;
    if (keyStr == "Numpad9")    return KeyCode::Numpad9;
    if (keyStr == "F1")         return KeyCode::F1;
    if (keyStr == "F2")         return KeyCode::F2;
    if (keyStr == "F3")         return KeyCode::F3;
    if (keyStr == "F4")         return KeyCode::F4;
    if (keyStr == "F5")         return KeyCode::F5;
    if (keyStr == "F6")         return KeyCode::F6;
    if (keyStr == "F7")         return KeyCode::F7;
    if (keyStr == "F8")         return KeyCode::F8;
    if (keyStr == "F9")         return KeyCode::F9;
    if (keyStr == "F10")        return KeyCode::F10;
    if (keyStr == "F11")        return KeyCode::F11;
    if (keyStr == "F12")        return KeyCode::F12;
    if (keyStr == "F13")        return KeyCode::F13;
    if (keyStr == "F14")        return KeyCode::F14;
    if (keyStr == "F15")        return KeyCode::F15;
    if (keyStr == "Pause")      return KeyCode::Pause;
    
    return KeyCode::Unknown;
}

std::string InputMapper::keyToString(KeyCode keyCode)
{
    switch (keyCode)
    {
        case KeyCode::A:            return "A";
        case KeyCode::B:            return "B";
        case KeyCode::C:            return "C";
        case KeyCode::D:            return "D";
        case KeyCode::E:            return "E";
        case KeyCode::F:            return "F";
        case KeyCode::G:            return "G";
        case KeyCode::H:            return "H";
        case KeyCode::I:            return "I";
        case KeyCode::J:            return "J";
        case KeyCode::K:            return "K";
        case KeyCode::L:            return "L";
        case KeyCode::M:            return "M";
        case KeyCode::N:            return "N";
        case KeyCode::O:            return "O";
        case KeyCode::P:            return "P";
        case KeyCode::Q:            return "Q";
        case KeyCode::R:            return "R";
        case KeyCode::S:            return "S";
        case KeyCode::T:            return "T";
        case KeyCode::U:            return "U";
        case KeyCode::V:            return "V";
        case KeyCode::W:            return "W";
        case KeyCode::X:            return "X";
        case KeyCode::Y:            return "Y";
        case KeyCode::Z:            return "Z";
        case KeyCode::Num0:         return "Num0";
        case KeyCode::Num1:         return "Num1";
        case KeyCode::Num2:         return "Num2";
        case KeyCode::Num3:         return "Num3";
        case KeyCode::Num4:         return "Num4";
        case KeyCode::Num5:         return "Num5";
        case KeyCode::Num6:         return "Num6";
        case KeyCode::Num7:         return "Num7";
        case KeyCode::Num8:         return "Num8";
        case KeyCode::Num9:         return "Num9";
        case KeyCode::Escape:       return "Escape";
        case KeyCode::LControl:     return "LControl";
        case KeyCode::LShift:       return "LShift";
        case KeyCode::LAlt:         return "LAlt";
        case KeyCode::LSystem:      return "LSystem";
        case KeyCode::RControl:     return "RControl";
        case KeyCode::RShift:       return "RShift";
        case KeyCode::RAlt:         return "RAlt";
        case KeyCode::RSystem:      return "RSystem";
        case KeyCode::Menu:         return "Menu";
        case KeyCode::LBracket:     return "LBracket";
        case KeyCode::RBracket:     return "RBracket";
        case KeyCode::Semicolon:    return "Semicolon";
        case KeyCode::Comma:        return "Comma";
        case KeyCode::Period:       return "Period";
        case KeyCode::Quote:        return "Quote";
        case KeyCode::Slash:        return "Slash";
        case KeyCode::Backslash:    return "Backslash";
        case KeyCode::Tilde:        return "Tilde";
        case KeyCode::Equal:        return "Equal";
        case KeyCode::Hyphen:       return "Hyphen";
        case KeyCode::Space:        return "Space";
        case KeyCode::Enter:        return "Enter";
        case KeyCode::Backspace:    return "Backspace";
        case KeyCode::Tab:          return "Tab";
        case KeyCode::PageUp:       return "PageUp";
        case KeyCode::PageDown:     return "PageDown";
        case KeyCode::End:          return "End";
        case KeyCode::Home:         return "Home";
        case KeyCode::Insert:       return "Insert";
        case KeyCode::Delete:       return "Delete";
        case KeyCode::Add:          return "Add";
        case KeyCode::Subtract:     return "Subtract";
        case KeyCode::Multiply:     return "Multiply";
        case KeyCode::Divide:       return "Divide";
        case KeyCode::Left:         return "Left";
        case KeyCode::Right:        return "Right";
        case KeyCode::Up:           return "Up";
        case KeyCode::Down:         return "Down";
        case KeyCode::Numpad0:      return "Numpad0";
        case KeyCode::Numpad1:      return "Numpad1";
        case KeyCode::Numpad2:      return "Numpad2";
        case KeyCode::Numpad3:      return "Numpad3";
        case KeyCode::Numpad4:      return "Numpad4";
        case KeyCode::Numpad5:      return "Numpad5";
        case KeyCode::Numpad6:      return "Numpad6";
        case KeyCode::Numpad7:      return "Numpad7";
        case KeyCode::Numpad8:      return "Numpad8";
        case KeyCode::Numpad9:      return "Numpad9";
        case KeyCode::F1:           return "F1";
        case KeyCode::F2:           return "F2";
        case KeyCode::F3:           return "F3";
        case KeyCode::F4:           return "F4";
        case KeyCode::F5:           return "F5";
        case KeyCode::F6:           return "F6";
        case KeyCode::F7:           return "F7";
        case KeyCode::F8:           return "F8";
        case KeyCode::F9:           return "F9";
        case KeyCode::F10:          return "F10";
        case KeyCode::F11:          return "F11";
        case KeyCode::F12:          return "F12";
        case KeyCode::F13:          return "F13";
        case KeyCode::F14:          return "F14";
        case KeyCode::F15:          return "F15";
        case KeyCode::Pause:        return "Pause";

        default: return "Unknown";
    }
}

MouseButton InputMapper::stringToMouseButton(const std::string& buttonStr)
{
    if (buttonStr == "Left")    return MouseButton::Left;
    if (buttonStr == "Right")   return MouseButton::Right;
    if (buttonStr == "Middle")  return MouseButton::Middle;
    if (buttonStr == "X1")      return MouseButton::X1;
    if (buttonStr == "X2")      return MouseButton::X2;
    
    return MouseButton::Unknown;
}

std::string InputMapper::mouseButtonToString(MouseButton button)
{
    switch (button)
    {
        case MouseButton::Left:     return "Left";
        case MouseButton::Right:    return "Right";
        case MouseButton::Middle:   return "Middle";
        case MouseButton::X1:       return "X1";
        case MouseButton::X2:       return "X2";
        default: return "Unknown";
    }
}

GamepadButton InputMapper::stringToGamepadButton(const std::string& buttonStr)
{
    if (buttonStr == "A")             return GamepadButton::A;
    if (buttonStr == "B")             return GamepadButton::B;
    if (buttonStr == "X")             return GamepadButton::X;
    if (buttonStr == "Y")             return GamepadButton::Y;
    if (buttonStr == "Back")          return GamepadButton::Back;
    if (buttonStr == "Guide")         return GamepadButton::Guide;
    if (buttonStr == "Start")         return GamepadButton::Start;
    if (buttonStr == "LeftStick")     return GamepadButton::LeftStick;
    if (buttonStr == "RightStick")    return GamepadButton::RightStick;
    if (buttonStr == "LeftShoulder")  return GamepadButton::LeftShoulder;
    if (buttonStr == "RightShoulder") return GamepadButton::RightShoulder;
    if (buttonStr == "DpadUp")        return GamepadButton::DpadUp;
    if (buttonStr == "DpadDown")      return GamepadButton::DpadDown;
    if (buttonStr == "DpadLeft")      return GamepadButton::DpadLeft;
    if (buttonStr == "DpadRight")     return GamepadButton::DpadRight;

    return GamepadButton::Unknown;
}

std::string InputMapper::gamepadButtonToString(GamepadButton button)
{
    switch (button)
    {
        case GamepadButton::A:             return "A";
        case GamepadButton::B:             return "B";
        case GamepadButton::X:             return "X";
        case GamepadButton::Y:             return "Y";
        case GamepadButton::Back:          return "Back";
        case GamepadButton::Guide:         return "Guide";
        case GamepadButton::Start:         return "Start";
        case GamepadButton::LeftStick:     return "LeftStick";
        case GamepadButton::RightStick:    return "RightStick";
        case GamepadButton::LeftShoulder:  return "LeftShoulder";
        case GamepadButton::RightShoulder: return "RightShoulder";
        case GamepadButton::DpadUp:        return "DpadUp";
        case GamepadButton::DpadDown:      return "DpadDown";
        case GamepadButton::DpadLeft:      return "DpadLeft";
        case GamepadButton::DpadRight:     return "DpadRight";

        default: return "Unknown";
    }
}

InputSource InputMapper::stringToSource(const std::string& sourceStr)
{
    size_t dot = sourceStr.find('.');
    if (dot == std::string::npos) throw std::runtime_error("Invalid source format: " + sourceStr);

    std::string device = sourceStr.substr(0, dot);
    std::string code = sourceStr.substr(dot + 1);

    if (device == "Keyboard")   return InputSource::keyboard(InputMapper::stringToKey(code));
    if (device == "Mouse")      return InputSource::mouse(InputMapper::stringToMouseButton(code));
    if (device == "Gamepad")    return InputSource::gamepad(InputMapper::stringToGamepadButton(code));
    
    throw std::runtime_error("Unknown device in source: " + device);
}

std::string InputMapper::sourceToString(const InputSource& source)
{
    std::string device, code;
    switch (source.type())
    {
        case InputSource::Type::Keyboard:
        {
            device = "Keyboard";
            code = InputMapper::keyToString(static_cast<KeyCode>(source.code()));
            break;
        }
        case InputSource::Type::Mouse:
        {
            device = "Mouse";
            code = InputMapper::mouseButtonToString(static_cast<MouseButton>(source.code()));
            break;
        }
        case InputSource::Type::Gamepad:
        {
            device = "Gamepad";
            code = InputMapper::gamepadButtonToString(static_cast<GamepadButton>(source.code()));
            break;
        }
        default: throw std::runtime_error("Unknown input source type");
    }
    return device + "." + code;
}
