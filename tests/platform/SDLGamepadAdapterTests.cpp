#include "../../src/platform/include/SDLGamepadAdapter/SDLGamepadAdapter.h"

#include "../../src/engine/events/InputEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputSource/InputSource.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <SDL.h>

#include <vector>

TEST_CASE("SDLGamepadAdapter emits button and axis events from a virtual joystick",
    "[integration][sdl_gamepad_adapter]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) == 0);
    SDL_JoystickEventState(SDL_ENABLE);

    const int deviceIndex = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 2, 2, 0);
    REQUIRE(deviceIndex >= 0);

    std::vector<DigitalInputEvent> receivedButtons;
    std::vector<AnalogInputEvent> receivedAxes;

    {
        EventBus bus;
        SDLGamepadAdapter adapter(bus, 7, deviceIndex);

        bus.subscribe<DigitalInputEvent>([&](const DigitalInputEvent& event)
        { receivedButtons.push_back(event); });

        bus.subscribe<AnalogInputEvent>([&](const AnalogInputEvent& event)
        { receivedAxes.push_back(event); });

        SDL_Event buttonDown {};
        buttonDown.type = SDL_JOYBUTTONDOWN;
        buttonDown.jbutton.button = SDL_CONTROLLER_BUTTON_A;

        SDL_Event buttonUp {};
        buttonUp.type = SDL_JOYBUTTONUP;
        buttonUp.jbutton.button = SDL_CONTROLLER_BUTTON_A;

        SDL_Event axisX {};
        axisX.type = SDL_JOYAXISMOTION;
        axisX.jaxis.axis = 0;
        axisX.jaxis.value = 16384;

        SDL_Event axisY {};
        axisY.type = SDL_JOYAXISMOTION;
        axisY.jaxis.axis = 1;
        axisY.jaxis.value = -16384;

        REQUIRE(SDL_PushEvent(&buttonDown) == 1);
        REQUIRE(SDL_PushEvent(&buttonUp) == 1);
        REQUIRE(SDL_PushEvent(&axisX) == 1);
        REQUIRE(SDL_PushEvent(&axisY) == 1);

        adapter.poll();
    }

    REQUIRE(receivedButtons.size() == 2);
    REQUIRE(receivedButtons[0].source == InputSource::gamepad(GamepadButton::A));
    REQUIRE(receivedButtons[0].playerId == 7);
    REQUIRE(receivedButtons[0].pressed == true);
    REQUIRE(receivedButtons[1].source == InputSource::gamepad(GamepadButton::A));
    REQUIRE(receivedButtons[1].playerId == 7);
    REQUIRE(receivedButtons[1].pressed == false);

    REQUIRE(receivedAxes.size() == 2);
    REQUIRE(receivedAxes[0].source == InputSource::gamepad(GamepadButton::LeftStick));
    REQUIRE(receivedAxes[0].playerId == 7);
    REQUIRE(receivedAxes[0].value == Catch::Approx(16384.0f / 32767.0f));
    REQUIRE(receivedAxes[1].source == InputSource::gamepad(GamepadButton::LeftStick));
    REQUIRE(receivedAxes[1].playerId == 7);
    REQUIRE(receivedAxes[1].value == Catch::Approx(-16384.0f / 32767.0f));

    REQUIRE(SDL_JoystickDetachVirtual(deviceIndex) == 0);
    SDL_Quit();
}
