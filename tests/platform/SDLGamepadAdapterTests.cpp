#include "platform/include/SDLGamepadAdapter/SDLGamepadAdapter.h"

#include "engine/events/InputEvent.h"
#include "engine/events/PlatformEvent.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/InputSource/InputSource.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <SDL.h>
#include <vector>

TEST_CASE("SDLGamepadAdapter emits button and axis events from a virtual joystick",
    "[integration][sdl_gamepad_adapter]"
) {
    REQUIRE(SDL_Init(SDL_INIT_EVENTS | SDL_INIT_JOYSTICK) == 0);

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

        std::vector<std::unique_ptr<PlatformEvent>> events;
        events.push_back(std::make_unique<GamepadButtonEvent>(static_cast<uint32_t>(GamepadButton::A), true));
        events.push_back(std::make_unique<GamepadButtonEvent>(static_cast<uint32_t>(GamepadButton::A), false));
        events.push_back(std::make_unique<GamepadAxisEvent>(0, 16384.f / 32767.f));
        events.push_back(std::make_unique<GamepadAxisEvent>(1, -16384.f / 32767.f));

        adapter.processEvents(events);
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
    REQUIRE(receivedAxes[0].value == Catch::Approx(16384.f / 32767.f));
    REQUIRE(receivedAxes[1].source == InputSource::gamepad(GamepadButton::LeftStick));
    REQUIRE(receivedAxes[1].playerId == 7);
    REQUIRE(receivedAxes[1].value == Catch::Approx(-16384.f / 32767.f));

    REQUIRE(SDL_JoystickDetachVirtual(deviceIndex) == 0);
    SDL_Quit();
}
