#include "../../../src/game/ui_actions/QuitAction.h"

#include "../../../src/engine/events/QuitEvent.h"
#include "../../../src/engine/include/EventBus/EventBus.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("QuitAction emits QuitEvent",
    "[unit][quit_action]"
) {
    EventBus bus;
    QuitAction action(bus);

    int quitCount = 0;
    bus.subscribe<QuitEvent>([&](const QuitEvent&) { ++quitCount; });

    action.execute();

    REQUIRE(quitCount == 1);
}
