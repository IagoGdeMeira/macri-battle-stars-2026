#include "../../src/platform/include/SDLMouseAdapter/SDLMouseAdapter.h"

#include "../../src/engine/events/InputEvent.h"
#include "../../src/engine/events/PlatformEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputSource/InputSource.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

TEST_CASE("SDLMouseAdapter emits button and motion events",
    "[integration][sdl_mouse_adapter]"
) {
    std::vector<DigitalInputEvent> receivedButtons;
    std::vector<AnalogInputEvent> receivedMotion;

    {
        EventBus bus;
        SDLMouseAdapter adapter(bus, 3);

        bus.subscribe<DigitalInputEvent>([&](const DigitalInputEvent& event)
        { receivedButtons.push_back(event); });

        bus.subscribe<AnalogInputEvent>([&](const AnalogInputEvent& event)
        { receivedMotion.push_back(event); });

        std::vector<std::unique_ptr<PlatformEvent>> events;
        events.push_back(std::make_unique<MouseButtonEvent>(static_cast<uint32_t>(MouseButton::Left), true));
        events.push_back(std::make_unique<MouseButtonEvent>(static_cast<uint32_t>(MouseButton::Left), false));
        events.push_back(std::make_unique<MouseMotionEvent>(Position{150.0f, 0.0f}, Position{0.0f, 0.0f}));
        events.push_back(std::make_unique<MouseMotionEvent>(Position{-250.0f, 0.0f}, Position{0.0f, 0.0f}));

        adapter.processEvents(events);
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
}
