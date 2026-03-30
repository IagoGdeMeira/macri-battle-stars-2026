#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include <SDL.h>

KeyCode SDLKeyTranslator::toKeyCode(int sdlKey)
{
    switch (sdlKey)
    {
        case SDLK_a: return KeyCode::A;
        case SDLK_b: return KeyCode::B;
        case SDLK_c: return KeyCode::C;
        case SDLK_d: return KeyCode::D;
        case SDLK_e: return KeyCode::E;
        case SDLK_f: return KeyCode::F;
        case SDLK_g: return KeyCode::G;
        case SDLK_h: return KeyCode::H;
        case SDLK_i: return KeyCode::I;
        case SDLK_j: return KeyCode::J;
        case SDLK_k: return KeyCode::K;
        case SDLK_l: return KeyCode::L;
        case SDLK_m: return KeyCode::M;
        case SDLK_n: return KeyCode::N;
        case SDLK_o: return KeyCode::O;
        case SDLK_p: return KeyCode::P;
        case SDLK_q: return KeyCode::Q;
        case SDLK_r: return KeyCode::R;
        case SDLK_s: return KeyCode::S;
        case SDLK_t: return KeyCode::T;
        case SDLK_u: return KeyCode::U;
        case SDLK_v: return KeyCode::V;
        case SDLK_w: return KeyCode::W;
        case SDLK_x: return KeyCode::X;
        case SDLK_y: return KeyCode::Y;
        case SDLK_z: return KeyCode::Z;
        case SDLK_0: return KeyCode::Num0;
        case SDLK_1: return KeyCode::Num1;
        case SDLK_2: return KeyCode::Num2;
        case SDLK_3: return KeyCode::Num3;
        case SDLK_4: return KeyCode::Num4;
        case SDLK_5: return KeyCode::Num5;
        case SDLK_6: return KeyCode::Num6;
        case SDLK_7: return KeyCode::Num7;
        case SDLK_8: return KeyCode::Num8;
        case SDLK_9: return KeyCode::Num9;
        case SDLK_ESCAPE: return KeyCode::Escape;
        case SDLK_LCTRL: return KeyCode::LControl;
        case SDLK_LSHIFT: return KeyCode::LShift;
        case SDLK_LALT: return KeyCode::LAlt;
        case SDLK_LGUI: return KeyCode::LSystem;
        case SDLK_RCTRL: return KeyCode::RControl;
        case SDLK_RSHIFT: return KeyCode::RShift;
        case SDLK_RALT: return KeyCode::RAlt;
        case SDLK_RGUI: return KeyCode::RSystem;
        case SDLK_MENU: return KeyCode::Menu;
        case SDLK_LEFTBRACKET: return KeyCode::LBracket;
        case SDLK_RIGHTBRACKET: return KeyCode::RBracket;
        case SDLK_SEMICOLON: return KeyCode::Semicolon;
        case SDLK_COMMA: return KeyCode::Comma;
        case SDLK_PERIOD: return KeyCode::Period;
        case SDLK_QUOTE: return KeyCode::Quote;
        case SDLK_SLASH: return KeyCode::Slash;
        case SDLK_BACKSLASH: return KeyCode::Backslash;
        case SDLK_BACKQUOTE: return KeyCode::Tilde;
        case SDLK_EQUALS: return KeyCode::Equal;
        case SDLK_MINUS: return KeyCode::Hyphen;
        case SDLK_SPACE: return KeyCode::Space;
        case SDLK_RETURN: return KeyCode::Enter;
        case SDLK_BACKSPACE: return KeyCode::Backspace;
        case SDLK_TAB: return KeyCode::Tab;
        case SDLK_PAGEUP: return KeyCode::PageUp;
        case SDLK_PAGEDOWN: return KeyCode::PageDown;
        case SDLK_END: return KeyCode::End;
        case SDLK_HOME: return KeyCode::Home;
        case SDLK_INSERT: return KeyCode::Insert;
        case SDLK_DELETE: return KeyCode::Delete;
        case SDLK_KP_PLUS: return KeyCode::Add;
        case SDLK_KP_MINUS: return KeyCode::Subtract;
        case SDLK_KP_MULTIPLY: return KeyCode::Multiply;
        case SDLK_KP_DIVIDE: return KeyCode::Divide;
        case SDLK_LEFT: return KeyCode::Left;
        case SDLK_RIGHT: return KeyCode::Right;
        case SDLK_UP: return KeyCode::Up;
        case SDLK_DOWN: return KeyCode::Down;
        case SDLK_KP_0: return KeyCode::Numpad0;
        case SDLK_KP_1: return KeyCode::Numpad1;
        case SDLK_KP_2: return KeyCode::Numpad2;
        case SDLK_KP_3: return KeyCode::Numpad3;
        case SDLK_KP_4: return KeyCode::Numpad4;
        case SDLK_KP_5: return KeyCode::Numpad5;
        case SDLK_KP_6: return KeyCode::Numpad6;
        case SDLK_KP_7: return KeyCode::Numpad7;
        case SDLK_KP_8: return KeyCode::Numpad8;
        case SDLK_KP_9: return KeyCode::Numpad9;
        case SDLK_F1: return KeyCode::F1;
        case SDLK_F2: return KeyCode::F2;
        case SDLK_F3: return KeyCode::F3;
        case SDLK_F4: return KeyCode::F4;
        case SDLK_F5: return KeyCode::F5;
        case SDLK_F6: return KeyCode::F6;
        case SDLK_F7: return KeyCode::F7;
        case SDLK_F8: return KeyCode::F8;
        case SDLK_F9: return KeyCode::F9;
        case SDLK_F10: return KeyCode::F10;
        case SDLK_F11: return KeyCode::F11;
        case SDLK_F12: return KeyCode::F12;
        case SDLK_F13: return KeyCode::F13;
        case SDLK_F14: return KeyCode::F14;
        case SDLK_F15: return KeyCode::F15;
        case SDLK_PAUSE: return KeyCode::Pause;
        
        default:
            return KeyCode::Unknown;
    }
}
