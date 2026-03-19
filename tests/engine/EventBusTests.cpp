#include "../../src/engine/include/EventBus/EventBus.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("EventBus can deliver events to subscribers",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct TestEvent { int value; };

    int received = 0;

    bus.subscribe<TestEvent>([&](const TestEvent& e) { received = e.value; });
    bus.emit<TestEvent>(42);

    REQUIRE(received == 42);
}

TEST_CASE("EventBus must notify multiple subscribers",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct TestEvent {};

    int count = 0;

    bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });
    bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    bus.emit<TestEvent>();

    REQUIRE(count == 2);
}

TEST_CASE("EventBus can emit with no subscribers does nothing",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct TestEvent {};

    REQUIRE_NOTHROW(bus.emit<TestEvent>());
}

TEST_CASE("EventBus can isolate event types",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct EventA {};
    struct EventB {};

    int countA = 0;
    int countB = 0;

    bus.subscribe<EventA>([&](const EventA&) { countA++; });
    bus.subscribe<EventB>([&](const EventB&) { countB++; });

    bus.emit<EventA>();

    REQUIRE(countA == 1);
    REQUIRE(countB == 0);
}

TEST_CASE("EventBus calls handlers in subscription order",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct TestEvent {};

    std::vector<int> order;

    bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(1); });
    bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(2); });
    bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(3); });

    bus.emit<TestEvent>();

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 3);
}

TEST_CASE("EventBus supports emitting events inside handler",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct EventA {};
    struct EventB {};

    int count = 0;

    bus.subscribe<EventA>([&](const EventA&) { bus.emit<EventB>(); });
    bus.subscribe<EventB>([&](const EventB&) { count++; });

    bus.emit<EventA>();

    REQUIRE(count == 1);
}

TEST_CASE("EventBus handles reentrant emit of same event",
    "[unit][event_bus]"
) {
    EventBus bus;

    struct TestEvent {};

    int count = 0;

    bus.subscribe<TestEvent>([&](const TestEvent&)
    {
        count++;
        if (count < 2) bus.emit<TestEvent>();
    });

    bus.emit<TestEvent>();

    REQUIRE(count == 2);
}
