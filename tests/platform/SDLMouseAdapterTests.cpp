#include "../../src/platform/include/SDLMouseAdapter/SDLMouseAdapter.h"

#include "../../src/engine/events/InputEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputSource/InputSource.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

#include <vector>

TEST_CASE("SDLMouseAdapter emits button and motion events",
    "[integration][sdl_mouse_adapter]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS) == 0);
    SDL_JoystickEventState(SDL_ENABLE);

    std::vector<DigitalInputEvent> receivedButtons;
    std::vector<AnalogInputEvent> receivedMotion;

    {
        EventBus bus;
        SDLMouseAdapter adapter(bus, 3);

        bus.subscribe<DigitalInputEvent>([&](const DigitalInputEvent& event)
        { receivedButtons.push_back(event); });

        bus.subscribe<AnalogInputEvent>([&](const AnalogInputEvent& event)
        { receivedMotion.push_back(event); });

        SDL_Event mouseDown {};
        mouseDown.type = SDL_MOUSEBUTTONDOWN;
        mouseDown.button.button = SDL_BUTTON_LEFT;

        SDL_Event mouseUp {};
        mouseUp.type = SDL_MOUSEBUTTONUP;
        mouseUp.button.button = SDL_BUTTON_LEFT;

        SDL_Event mouseMotionPositive {};
        mouseMotionPositive.type = SDL_MOUSEMOTION;
        mouseMotionPositive.motion.xrel = 150;

        SDL_Event mouseMotionNegative {};
        mouseMotionNegative.type = SDL_MOUSEMOTION;
        mouseMotionNegative.motion.xrel = -250;

        REQUIRE(SDL_PushEvent(&mouseDown) == 1);
        REQUIRE(SDL_PushEvent(&mouseUp) == 1);
        REQUIRE(SDL_PushEvent(&mouseMotionPositive) == 1);
        REQUIRE(SDL_PushEvent(&mouseMotionNegative) == 1);

        adapter.poll();
    }

    REQUIRE(receivedButtons.size() == 2);
    REQUIRE(receivedButtons[0].source == InputSource::mouse(MouseButton::Left));
    REQUIRE(receivedButtons[0].playerId == 3);
    REQUIRE(receivedButtons[0].pressed == true);
    REQUIRE(receivedButtons[1].source == InputSource::mouse(MouseButton::Left));
    REQUIRE(receivedButtons[1].playerId == 3);
    REQUIRE(receivedButtons[1].pressed == false);

    REQUIRE(receivedMotion.size() == 2);
    REQUIRE(receivedMotion[0].source == InputSource::mouse(MouseButton::Left));
    REQUIRE(receivedMotion[0].playerId == 3);
    REQUIRE(receivedMotion[0].value == Catch::Approx(1.0f));
    REQUIRE(receivedMotion[1].source == InputSource::mouse(MouseButton::Left));
    REQUIRE(receivedMotion[1].playerId == 3);
    REQUIRE(receivedMotion[1].value == Catch::Approx(-1.0f));

    SDL_Quit();
}
