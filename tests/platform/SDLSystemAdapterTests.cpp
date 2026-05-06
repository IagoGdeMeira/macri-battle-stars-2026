#include "../../src/platform/include/SDLSystemAdapter/SDLSystemAdapter.h"

#include "../../src/engine/events/PlatformEvent.h"
#include "../../src/engine/events/QuitEvent.h"
#include "../../src/engine/include/EventBus/EventBus.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

TEST_CASE("SDLSystemAdapter emits QuitEvent for QuitPlatformEvent",
    "[integration][sdl_system_adapter]"
) {
    EventBus bus;
    SDLSystemAdapter adapter(bus);

    int quitCount = 0;
    bus.subscribe<QuitEvent>([&](const QuitEvent&) { ++quitCount; });

    std::vector<std::unique_ptr<PlatformEvent>> events;
    events.push_back(std::make_unique<QuitPlatformEvent>());
    events.push_back(std::make_unique<KeyboardEvent>(0, true));

    adapter.processEvents(events);

    REQUIRE(quitCount == 1);
}
