#include "../../src/platform/include/SDLKeyTranslator/SDLKeyTranslator.h"

#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

TEST_CASE("SDLKeyTranslator maps letters, digits and control keys",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_a) == KeyCode::A);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_z) == KeyCode::Z);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_0) == KeyCode::Num0);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_9) == KeyCode::Num9);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_ESCAPE) == KeyCode::Escape);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LCTRL) == KeyCode::LControl);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LSHIFT) == KeyCode::LShift);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LALT) == KeyCode::LAlt);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LGUI) == KeyCode::LSystem);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RCTRL) == KeyCode::RControl);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RSHIFT) == KeyCode::RShift);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RALT) == KeyCode::RAlt);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RGUI) == KeyCode::RSystem);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_MENU) == KeyCode::Menu);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LEFTBRACKET) == KeyCode::LBracket);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RIGHTBRACKET) == KeyCode::RBracket);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_SEMICOLON) == KeyCode::Semicolon);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_COMMA) == KeyCode::Comma);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_PERIOD) == KeyCode::Period);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_QUOTE) == KeyCode::Quote);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_SLASH) == KeyCode::Slash);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_BACKSLASH) == KeyCode::Backslash);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_BACKQUOTE) == KeyCode::Tilde);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_EQUALS) == KeyCode::Equal);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_MINUS) == KeyCode::Hyphen);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_SPACE) == KeyCode::Space);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_BACKSPACE) == KeyCode::Backspace);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_TAB) == KeyCode::Tab);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_PAGEUP) == KeyCode::PageUp);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_PAGEDOWN) == KeyCode::PageDown);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_END) == KeyCode::End);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_HOME) == KeyCode::Home);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_INSERT) == KeyCode::Insert);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_DELETE) == KeyCode::Delete);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_PLUS) == KeyCode::Add);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_MINUS) == KeyCode::Subtract);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_MULTIPLY) == KeyCode::Multiply);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_DIVIDE) == KeyCode::Divide);
}

TEST_CASE("SDLKeyTranslator maps navigation and function keys",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LEFT) == KeyCode::Left);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RIGHT) == KeyCode::Right);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_UP) == KeyCode::Up);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_DOWN) == KeyCode::Down);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_0) == KeyCode::Numpad0);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_1) == KeyCode::Numpad1);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_2) == KeyCode::Numpad2);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_3) == KeyCode::Numpad3);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_4) == KeyCode::Numpad4);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_5) == KeyCode::Numpad5);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_6) == KeyCode::Numpad6);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_7) == KeyCode::Numpad7);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_8) == KeyCode::Numpad8);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_KP_9) == KeyCode::Numpad9);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F1) == KeyCode::F1);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F12) == KeyCode::F12);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F13) == KeyCode::F13);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F14) == KeyCode::F14);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F15) == KeyCode::F15);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_PAUSE) == KeyCode::Pause);
}

TEST_CASE("SDLKeyTranslator returns unknown for unmapped keys",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toKeyCode(-123456) == KeyCode::Unknown);
    REQUIRE(SDLKeyTranslator::toKeyCode(999999) == KeyCode::Unknown);
}

TEST_CASE("SDLKeyTranslator maps gamepad buttons",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_A) == GamepadButton::A);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_B) == GamepadButton::B);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_X) == GamepadButton::X);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_Y) == GamepadButton::Y);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_BACK) == GamepadButton::Back);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_START) == GamepadButton::Start);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == GamepadButton::LeftShoulder);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == GamepadButton::RightShoulder);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_UP) == GamepadButton::DpadUp);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) == GamepadButton::DpadDown);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) == GamepadButton::DpadLeft);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == GamepadButton::DpadRight);
}

TEST_CASE("SDLKeyTranslator returns unknown for unmapped gamepad buttons",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toGamepadButton(-123456) == GamepadButton::Unknown);
    REQUIRE(SDLKeyTranslator::toGamepadButton(999999) == GamepadButton::Unknown);
}

TEST_CASE("SDLKeyTranslator maps mouse buttons",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toMouseButton(SDL_BUTTON_LEFT) == MouseButton::Left);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDL_BUTTON_RIGHT) == MouseButton::Right);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDL_BUTTON_MIDDLE) == MouseButton::Middle);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDL_BUTTON_X1) == MouseButton::X1);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDL_BUTTON_X2) == MouseButton::X2);
}

TEST_CASE("SDLKeyTranslator returns unknown for unmapped mouse buttons",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toMouseButton(-123456) == MouseButton::Unknown);
    REQUIRE(SDLKeyTranslator::toMouseButton(999999) == MouseButton::Unknown);
}

TEST_CASE("SDLKeyTranslator reverses KeyCode conversions",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::A)) == KeyCode::A);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Z)) == KeyCode::Z);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::M)) == KeyCode::M);

    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Num0)) == KeyCode::Num0);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Num5)) == KeyCode::Num5);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Num9)) == KeyCode::Num9);

    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F1)) == KeyCode::F1);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F12)) == KeyCode::F12);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F15)) == KeyCode::F15);

    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Escape)) == KeyCode::Escape);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::LShift)) == KeyCode::LShift);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::LAlt)) == KeyCode::LAlt);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::LSystem)) == KeyCode::LSystem);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::RControl)) == KeyCode::RControl);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::RShift)) == KeyCode::RShift);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::RAlt)) == KeyCode::RAlt);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::RSystem)) == KeyCode::RSystem);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Menu)) == KeyCode::Menu);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::LBracket)) == KeyCode::LBracket);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::RBracket)) == KeyCode::RBracket);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Semicolon)) == KeyCode::Semicolon);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Comma)) == KeyCode::Comma);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Period)) == KeyCode::Period);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Quote)) == KeyCode::Quote);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Slash)) == KeyCode::Slash);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Backslash)) == KeyCode::Backslash);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Tilde)) == KeyCode::Tilde);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Equal)) == KeyCode::Equal);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Hyphen)) == KeyCode::Hyphen);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Space)) == KeyCode::Space);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Enter)) == KeyCode::Enter);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Tab)) == KeyCode::Tab);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::PageUp)) == KeyCode::PageUp);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::PageDown)) == KeyCode::PageDown);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::End)) == KeyCode::End);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Home)) == KeyCode::Home);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Insert)) == KeyCode::Insert);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Backspace)) == KeyCode::Backspace);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Delete)) == KeyCode::Delete);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Add)) == KeyCode::Add);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Subtract)) == KeyCode::Subtract);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Multiply)) == KeyCode::Multiply);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Divide)) == KeyCode::Divide);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Left)) == KeyCode::Left);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Right)) == KeyCode::Right);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Up)) == KeyCode::Up);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Down)) == KeyCode::Down);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Numpad0)) == KeyCode::Numpad0);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Numpad5)) == KeyCode::Numpad5);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Numpad9)) == KeyCode::Numpad9);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F13)) == KeyCode::F13);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F14)) == KeyCode::F14);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::F15)) == KeyCode::F15);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLKeyTranslator::fromKeyCode(KeyCode::Pause)) == KeyCode::Pause);
}

TEST_CASE("SDLKeyTranslator reverses gamepad button conversions",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::A)) == GamepadButton::A);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::B)) == GamepadButton::B);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::X)) == GamepadButton::X);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::Y)) == GamepadButton::Y);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::Back)) == GamepadButton::Back);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::Start)) == GamepadButton::Start);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::LeftStick)) == GamepadButton::LeftStick);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::RightStick)) == GamepadButton::RightStick);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::LeftShoulder)) == GamepadButton::LeftShoulder);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::RightShoulder)) == GamepadButton::RightShoulder);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::DpadUp)) == GamepadButton::DpadUp);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::DpadDown)) == GamepadButton::DpadDown);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::DpadLeft)) == GamepadButton::DpadLeft);
    REQUIRE(SDLKeyTranslator::toGamepadButton(SDLKeyTranslator::fromGamepadButton(GamepadButton::DpadRight)) == GamepadButton::DpadRight);
}

TEST_CASE("SDLKeyTranslator reverses mouse button conversions",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toMouseButton(SDLKeyTranslator::fromMouseButton(MouseButton::Left)) == MouseButton::Left);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDLKeyTranslator::fromMouseButton(MouseButton::Right)) == MouseButton::Right);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDLKeyTranslator::fromMouseButton(MouseButton::Middle)) == MouseButton::Middle);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDLKeyTranslator::fromMouseButton(MouseButton::X1)) == MouseButton::X1);
    REQUIRE(SDLKeyTranslator::toMouseButton(SDLKeyTranslator::fromMouseButton(MouseButton::X2)) == MouseButton::X2);
}

TEST_CASE("SDLKeyTranslator returns unknown for invalid reverse conversions",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::fromKeyCode(KeyCode::Unknown) == SDLK_UNKNOWN);
    REQUIRE(SDLKeyTranslator::fromMouseButton(MouseButton::Unknown) == SDLK_UNKNOWN);
    REQUIRE(SDLKeyTranslator::fromGamepadButton(GamepadButton::Unknown) == SDLK_UNKNOWN);
}
