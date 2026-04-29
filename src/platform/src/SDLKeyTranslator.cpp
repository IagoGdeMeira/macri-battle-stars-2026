#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include <SDL.h>

KeyCode SDLKeyTranslator::toKeyCode(int sdlKey)
{
    if (sdlKey >= SDLK_a && sdlKey <= SDLK_z)
    { return static_cast<KeyCode>(static_cast<int>(KeyCode::A) + (sdlKey - SDLK_a)); }

    if (sdlKey >= SDLK_0 && sdlKey <= SDLK_9)
    { return static_cast<KeyCode>(static_cast<int>(KeyCode::Num0) + (sdlKey - SDLK_0)); }

    switch (sdlKey)
    {
        case SDLK_ESCAPE:       return KeyCode::Escape;
        case SDLK_LCTRL:        return KeyCode::LControl;
        case SDLK_LSHIFT:       return KeyCode::LShift;
        case SDLK_LALT:         return KeyCode::LAlt;
        case SDLK_LGUI:         return KeyCode::LSystem;
        case SDLK_RCTRL:        return KeyCode::RControl;
        case SDLK_RSHIFT:       return KeyCode::RShift;
        case SDLK_RALT:         return KeyCode::RAlt;
        case SDLK_RGUI:         return KeyCode::RSystem;
        case SDLK_MENU:         return KeyCode::Menu;
        case SDLK_LEFTBRACKET:  return KeyCode::LBracket;
        case SDLK_RIGHTBRACKET: return KeyCode::RBracket;
        case SDLK_SEMICOLON:    return KeyCode::Semicolon;
        case SDLK_COMMA:        return KeyCode::Comma;
        case SDLK_PERIOD:       return KeyCode::Period;
        case SDLK_QUOTE:        return KeyCode::Quote;
        case SDLK_SLASH:        return KeyCode::Slash;
        case SDLK_BACKSLASH:    return KeyCode::Backslash;
        case SDLK_BACKQUOTE:    return KeyCode::Tilde;
        case SDLK_EQUALS:       return KeyCode::Equal;
        case SDLK_MINUS:        return KeyCode::Hyphen;
        case SDLK_SPACE:        return KeyCode::Space;
        case SDLK_RETURN:       return KeyCode::Enter;
        case SDLK_BACKSPACE:    return KeyCode::Backspace;
        case SDLK_TAB:          return KeyCode::Tab;
        case SDLK_PAGEUP:       return KeyCode::PageUp;
        case SDLK_PAGEDOWN:     return KeyCode::PageDown;
        case SDLK_END:          return KeyCode::End;
        case SDLK_HOME:         return KeyCode::Home;
        case SDLK_INSERT:       return KeyCode::Insert;
        case SDLK_DELETE:       return KeyCode::Delete;
        case SDLK_KP_PLUS:      return KeyCode::Add;
        case SDLK_KP_MINUS:     return KeyCode::Subtract;
        case SDLK_KP_MULTIPLY:  return KeyCode::Multiply;
        case SDLK_KP_DIVIDE:    return KeyCode::Divide;
        case SDLK_LEFT:         return KeyCode::Left;
        case SDLK_RIGHT:        return KeyCode::Right;
        case SDLK_UP:           return KeyCode::Up;
        case SDLK_DOWN:         return KeyCode::Down;
        case SDLK_KP_0:         return KeyCode::Numpad0;
        case SDLK_KP_1:         return KeyCode::Numpad1;
        case SDLK_KP_2:         return KeyCode::Numpad2;
        case SDLK_KP_3:         return KeyCode::Numpad3;
        case SDLK_KP_4:         return KeyCode::Numpad4;
        case SDLK_KP_5:         return KeyCode::Numpad5;
        case SDLK_KP_6:         return KeyCode::Numpad6;
        case SDLK_KP_7:         return KeyCode::Numpad7;
        case SDLK_KP_8:         return KeyCode::Numpad8;
        case SDLK_KP_9:         return KeyCode::Numpad9;
        case SDLK_F1:           return KeyCode::F1;
        case SDLK_F2:           return KeyCode::F2;
        case SDLK_F3:           return KeyCode::F3;
        case SDLK_F4:           return KeyCode::F4;
        case SDLK_F5:           return KeyCode::F5;
        case SDLK_F6:           return KeyCode::F6;
        case SDLK_F7:           return KeyCode::F7;
        case SDLK_F8:           return KeyCode::F8;
        case SDLK_F9:           return KeyCode::F9;
        case SDLK_F10:          return KeyCode::F10;
        case SDLK_F11:          return KeyCode::F11;
        case SDLK_F12:          return KeyCode::F12;
        case SDLK_F13:          return KeyCode::F13;
        case SDLK_F14:          return KeyCode::F14;
        case SDLK_F15:          return KeyCode::F15;
        case SDLK_PAUSE:        return KeyCode::Pause;
        
        default: return KeyCode::Unknown;
    }
}

SDL_Keycode SDLKeyTranslator::fromKeyCode(KeyCode key)
{
    switch (key)
    {
        case KeyCode::A:            return SDLK_a;
        case KeyCode::B:            return SDLK_b;
        case KeyCode::C:            return SDLK_c;
        case KeyCode::D:            return SDLK_d;
        case KeyCode::E:            return SDLK_e;
        case KeyCode::F:            return SDLK_f;
        case KeyCode::G:            return SDLK_g;
        case KeyCode::H:            return SDLK_h;
        case KeyCode::I:            return SDLK_i;
        case KeyCode::J:            return SDLK_j;
        case KeyCode::K:            return SDLK_k;
        case KeyCode::L:            return SDLK_l;
        case KeyCode::M:            return SDLK_m;
        case KeyCode::N:            return SDLK_n;
        case KeyCode::O:            return SDLK_o;
        case KeyCode::P:            return SDLK_p;
        case KeyCode::Q:            return SDLK_q;
        case KeyCode::R:            return SDLK_r;
        case KeyCode::S:            return SDLK_s;
        case KeyCode::T:            return SDLK_t;
        case KeyCode::U:            return SDLK_u;
        case KeyCode::V:            return SDLK_v;
        case KeyCode::W:            return SDLK_w;
        case KeyCode::X:            return SDLK_x;
        case KeyCode::Y:            return SDLK_y;
        case KeyCode::Z:            return SDLK_z;
        case KeyCode::Num0:         return SDLK_0;
        case KeyCode::Num1:         return SDLK_1;
        case KeyCode::Num2:         return SDLK_2;
        case KeyCode::Num3:         return SDLK_3;
        case KeyCode::Num4:         return SDLK_4;
        case KeyCode::Num5:         return SDLK_5;
        case KeyCode::Num6:         return SDLK_6;
        case KeyCode::Num7:         return SDLK_7;
        case KeyCode::Num8:         return SDLK_8;
        case KeyCode::Num9:         return SDLK_9;
        case KeyCode::Escape:       return SDLK_ESCAPE;
        case KeyCode::LControl:     return SDLK_LCTRL;
        case KeyCode::LShift:       return SDLK_LSHIFT;
        case KeyCode::LAlt:         return SDLK_LALT;
        case KeyCode::LSystem:      return SDLK_LGUI;
        case KeyCode::RControl:     return SDLK_RCTRL;
        case KeyCode::RShift:       return SDLK_RSHIFT;
        case KeyCode::RAlt:         return SDLK_RALT;
        case KeyCode::RSystem:      return SDLK_RGUI;
        case KeyCode::Menu:         return SDLK_MENU;
        case KeyCode::LBracket:     return SDLK_LEFTBRACKET;
        case KeyCode::RBracket:     return SDLK_RIGHTBRACKET;
        case KeyCode::Semicolon:    return SDLK_SEMICOLON;
        case KeyCode::Comma:        return SDLK_COMMA;
        case KeyCode::Period:       return SDLK_PERIOD;
        case KeyCode::Quote:        return SDLK_QUOTE;
        case KeyCode::Slash:        return SDLK_SLASH;
        case KeyCode::Backslash:    return SDLK_BACKSLASH;
        case KeyCode::Tilde:        return SDLK_BACKQUOTE;
        case KeyCode::Equal:        return SDLK_EQUALS;
        case KeyCode::Hyphen:       return SDLK_MINUS;
        case KeyCode::Space:        return SDLK_SPACE;
        case KeyCode::Enter:        return SDLK_RETURN;
        case KeyCode::Backspace:    return SDLK_BACKSPACE;
        case KeyCode::Tab:          return SDLK_TAB;
        case KeyCode::PageUp:       return SDLK_PAGEUP;
        case KeyCode::PageDown:     return SDLK_PAGEDOWN;
        case KeyCode::End:          return SDLK_END;
        case KeyCode::Home:         return SDLK_HOME;
        case KeyCode::Insert:       return SDLK_INSERT;
        case KeyCode::Delete:       return SDLK_DELETE;
        case KeyCode::Add:          return SDLK_KP_PLUS;
        case KeyCode::Subtract:     return SDLK_KP_MINUS;
        case KeyCode::Multiply:     return SDLK_KP_MULTIPLY;
        case KeyCode::Divide:       return SDLK_KP_DIVIDE;
        case KeyCode::Left:         return SDLK_LEFT;
        case KeyCode::Right:        return SDLK_RIGHT;
        case KeyCode::Up:           return SDLK_UP;
        case KeyCode::Down:         return SDLK_DOWN;
        case KeyCode::Numpad0:      return SDLK_KP_0;
        case KeyCode::Numpad1:      return SDLK_KP_1;
        case KeyCode::Numpad2:      return SDLK_KP_2;
        case KeyCode::Numpad3:      return SDLK_KP_3;
        case KeyCode::Numpad4:      return SDLK_KP_4;
        case KeyCode::Numpad5:      return SDLK_KP_5;
        case KeyCode::Numpad6:      return SDLK_KP_6;
        case KeyCode::Numpad7:      return SDLK_KP_7;
        case KeyCode::Numpad8:      return SDLK_KP_8;
        case KeyCode::Numpad9:      return SDLK_KP_9;
        case KeyCode::F1:           return SDLK_F1;
        case KeyCode::F2:           return SDLK_F2;
        case KeyCode::F3:           return SDLK_F3;
        case KeyCode::F4:           return SDLK_F4;
        case KeyCode::F5:           return SDLK_F5;
        case KeyCode::F6:           return SDLK_F6;
        case KeyCode::F7:           return SDLK_F7;
        case KeyCode::F8:           return SDLK_F8;
        case KeyCode::F9:           return SDLK_F9;
        case KeyCode::F10:          return SDLK_F10;
        case KeyCode::F11:          return SDLK_F11;
        case KeyCode::F12:          return SDLK_F12;
        case KeyCode::F13:          return SDLK_F13;
        case KeyCode::F14:          return SDLK_F14;
        case KeyCode::F15:          return SDLK_F15;
        case KeyCode::Pause:        return SDLK_PAUSE;
        
        default: return SDLK_UNKNOWN;
    }
}

MouseButton SDLKeyTranslator::toMouseButton(int sdlButton)
{
    switch (sdlButton)
    {
        case SDL_BUTTON_LEFT:   return MouseButton::Left;
        case SDL_BUTTON_RIGHT:  return MouseButton::Right;
        case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
        case SDL_BUTTON_X1:     return MouseButton::X1;
        case SDL_BUTTON_X2:     return MouseButton::X2;
        
        default: return MouseButton::Unknown;
    }
}

int SDLKeyTranslator::fromMouseButton(MouseButton button)
{
    switch (button)
    {
        case MouseButton::Left:   return SDL_BUTTON_LEFT;
        case MouseButton::Right:  return SDL_BUTTON_RIGHT;
        case MouseButton::Middle: return SDL_BUTTON_MIDDLE;
        case MouseButton::X1:     return SDL_BUTTON_X1;
        case MouseButton::X2:     return SDL_BUTTON_X2;
        
        default: return SDLK_UNKNOWN;
    }
}

GamepadButton SDLKeyTranslator::toGamepadButton(int sdlButton)
{
    switch (sdlButton)
    {
        case SDL_CONTROLLER_BUTTON_A:               return GamepadButton::A;
        case SDL_CONTROLLER_BUTTON_B:               return GamepadButton::B;
        case SDL_CONTROLLER_BUTTON_X:               return GamepadButton::X;
        case SDL_CONTROLLER_BUTTON_Y:               return GamepadButton::Y;
        case SDL_CONTROLLER_BUTTON_BACK:            return GamepadButton::Back;
        case SDL_CONTROLLER_BUTTON_GUIDE:           return GamepadButton::Guide;
        case SDL_CONTROLLER_BUTTON_START:           return GamepadButton::Start;
        case SDL_CONTROLLER_BUTTON_LEFTSTICK:       return GamepadButton::LeftStick;
        case SDL_CONTROLLER_BUTTON_RIGHTSTICK:      return GamepadButton::RightStick;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:    return GamepadButton::LeftShoulder;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:   return GamepadButton::RightShoulder;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:         return GamepadButton::DpadUp;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:       return GamepadButton::DpadDown;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:       return GamepadButton::DpadLeft;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:      return GamepadButton::DpadRight;
        
        default: return GamepadButton::Unknown;
    }
}

int SDLKeyTranslator::fromGamepadButton(GamepadButton button)
{
    switch (button)
    {
        case GamepadButton::A:             return SDL_CONTROLLER_BUTTON_A;
        case GamepadButton::B:             return SDL_CONTROLLER_BUTTON_B;
        case GamepadButton::X:             return SDL_CONTROLLER_BUTTON_X;
        case GamepadButton::Y:             return SDL_CONTROLLER_BUTTON_Y;
        case GamepadButton::Back:          return SDL_CONTROLLER_BUTTON_BACK;
        case GamepadButton::Guide:         return SDL_CONTROLLER_BUTTON_GUIDE;
        case GamepadButton::Start:         return SDL_CONTROLLER_BUTTON_START;
        case GamepadButton::LeftStick:     return SDL_CONTROLLER_BUTTON_LEFTSTICK;
        case GamepadButton::RightStick:    return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
        case GamepadButton::LeftShoulder:  return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
        case GamepadButton::RightShoulder: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
        case GamepadButton::DpadUp:        return SDL_CONTROLLER_BUTTON_DPAD_UP;
        case GamepadButton::DpadDown:      return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        case GamepadButton::DpadLeft:      return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        case GamepadButton::DpadRight:     return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        
        default: return SDLK_UNKNOWN;
    }
}
