#include "../../src/platform/include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"

#include "../../src/engine/events/InputEvent.h"
#include "../../src/engine/events/PlatformEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/InputBinding/InputBinding.h"
#include "../../src/engine/include/InputContext/InputContext.h"
#include "../../src/engine/include/KeyCode/KeyCode.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

TEST_CASE("SDLKeyboardAdapter emits quit and digital input events",
    "[integration][sdl_keyboard_adapter]"
) {
    InputContext inputContext;
    InputBinding binding;
    binding.keyMap[InputSource::keyboard(KeyCode::A)] = InputAction::MoveLeft;
    inputContext.bindings[0] = binding;

    EventBus bus;
    SDLKeyboardAdapter adapter(bus, inputContext);

    std::vector<DigitalInputEvent> receivedInputs;

    bus.subscribe<DigitalInputEvent>([&](const DigitalInputEvent& event)
    { receivedInputs.push_back(event); });

    std::vector<std::unique_ptr<PlatformEvent>> events;
    events.push_back(std::make_unique<KeyboardEvent>(static_cast<uint32_t>(KeyCode::A), true));
    events.push_back(std::make_unique<KeyboardEvent>(static_cast<uint32_t>(KeyCode::A), false));

    adapter.processEvents(events);

    REQUIRE(receivedInputs.size() == 2);
    REQUIRE(receivedInputs[0].source == InputSource::keyboard(KeyCode::A));
    REQUIRE(receivedInputs[0].playerId == 0);
    REQUIRE(receivedInputs[0].pressed == true);
    REQUIRE(receivedInputs[1].source == InputSource::keyboard(KeyCode::A));
    REQUIRE(receivedInputs[1].playerId == 0);
    REQUIRE(receivedInputs[1].pressed == false);
}
