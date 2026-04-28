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
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RSHIFT) == KeyCode::RShift);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_SPACE) == KeyCode::Space);
}

TEST_CASE("SDLKeyTranslator maps navigation and function keys",
    "[unit][sdl_key_translator]"
) {
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_LEFT) == KeyCode::Left);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_RIGHT) == KeyCode::Right);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_UP) == KeyCode::Up);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_DOWN) == KeyCode::Down);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F1) == KeyCode::F1);
    REQUIRE(SDLKeyTranslator::toKeyCode(SDLK_F12) == KeyCode::F12);
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