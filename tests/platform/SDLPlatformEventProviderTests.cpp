#include "../../src/platform/include/SDLPlatformEventProvider/SDLPlatformEventProvider.h"

#include "../../src/engine/events/PlatformEvent.h"
#include "../../src/engine/include/GamepadButton/GamepadButton.h"
#include "../../src/engine/include/KeyCode/KeyCode.h"
#include "../../src/engine/include/MouseButton/MouseButton.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <SDL.h>
#include <vector>

TEST_CASE("SDLPlatformEventProvider converts SDL events into platform events",
    "[integration][sdl_platform_event_provider]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) == 0);

    SDL_Event keyDown {};
    keyDown.type = SDL_KEYDOWN;
    keyDown.key.keysym.sym = SDLK_a;

    SDL_Event mouseDown {};
    mouseDown.type = SDL_MOUSEBUTTONDOWN;
    mouseDown.button.button = SDL_BUTTON_LEFT;

    SDL_Event mouseMotion {};
    mouseMotion.type = SDL_MOUSEMOTION;
    mouseMotion.motion.xrel = 150;
    mouseMotion.motion.yrel = -25;

    SDL_Event joyButtonDown {};
    joyButtonDown.type = SDL_JOYBUTTONDOWN;
    joyButtonDown.jbutton.button = SDL_CONTROLLER_BUTTON_A;

    SDL_Event joyAxisMotion {};
    joyAxisMotion.type = SDL_JOYAXISMOTION;
    joyAxisMotion.jaxis.axis = 0;
    joyAxisMotion.jaxis.value = 16384;

    SDL_Event quitEvent {};
    quitEvent.type = SDL_QUIT;

    REQUIRE(SDL_PushEvent(&keyDown) == 1);
    REQUIRE(SDL_PushEvent(&mouseDown) == 1);
    REQUIRE(SDL_PushEvent(&mouseMotion) == 1);
    REQUIRE(SDL_PushEvent(&joyButtonDown) == 1);
    REQUIRE(SDL_PushEvent(&joyAxisMotion) == 1);
    REQUIRE(SDL_PushEvent(&quitEvent) == 1);

    SDLPlatformEventProvider provider;
    const auto events = provider.pollEvents();

    REQUIRE(events.size() == 6);
    REQUIRE(events[0]->type() == PlatformEvent::Type::Keyboard);
    REQUIRE(dynamic_cast<KeyboardEvent*>(events[0].get())->pressed == true);
    REQUIRE(dynamic_cast<KeyboardEvent*>(events[0].get())->keyCode == static_cast<uint32_t>(KeyCode::A));
    
    REQUIRE(events[1]->type() == PlatformEvent::Type::Mouse);
    REQUIRE(dynamic_cast<MouseButtonEvent*>(events[1].get())->pressed == true);
    REQUIRE(dynamic_cast<MouseButtonEvent*>(events[1].get())->button == static_cast<uint32_t>(MouseButton::Left));

    REQUIRE(events[2]->type() == PlatformEvent::Type::Mouse);
    REQUIRE(dynamic_cast<MouseMotionEvent*>(events[2].get())->delta.x == Catch::Approx(150.f).margin(0.01f));
    REQUIRE(dynamic_cast<MouseMotionEvent*>(events[2].get())->delta.y == Catch::Approx(-25.f).margin(0.01f));

    REQUIRE(events[3]->type() == PlatformEvent::Type::Gamepad);
    REQUIRE(dynamic_cast<GamepadButtonEvent*>(events[3].get())->pressed == true);

    REQUIRE(events[4]->type() == PlatformEvent::Type::Gamepad);
    REQUIRE(dynamic_cast<GamepadAxisEvent*>(events[4].get())->value == Catch::Approx(16384.f / 32767.f).margin(0.01f));

    REQUIRE(events[5]->type() == PlatformEvent::Type::Quit);

    SDL_Quit();
}