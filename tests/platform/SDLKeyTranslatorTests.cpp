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