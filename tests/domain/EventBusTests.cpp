#include "../../src/domain/include/EventBus/EventBus.h"

#include "../../src/domain/include/EventDispatcher/EventDispatcher.h"

#include <catch2/catch_test_macros.hpp>

struct TestEvent { int value; };
struct EventBusFixture { EventBus bus; };

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus delivers event to subscribed handler",
    "[unit][event_bus]"
) {
    int received = 0;

    this->bus.subscribe<TestEvent>([&](const TestEvent& e) { received = e.value; });

    EventDispatcher dispatcher(this->bus);
    dispatcher.post<TestEvent>({42});
    dispatcher.process();

    REQUIRE(received == 42);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus calls handlers in subscription order",
    "[unit][event_bus]"
) {
    std::vector<int> order;

    this->bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(1); });
    this->bus.subscribe<TestEvent>([&](const TestEvent&) { order.push_back(2); });

    EventDispatcher dispatcher(this->bus);
    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(order.size() == 2);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus unsubscribe removes handler",
    "[unit][event_bus]"
) {
    int count = 0;

    auto id = this->bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    this->bus.unsubscribe<TestEvent>(id);

    EventDispatcher dispatcher(this->bus);
    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 0);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus clear removes all handlers",
    "[unit][event_bus]"
) {
    int count = 0;

    this->bus.subscribe<TestEvent>([&](const TestEvent&) { count++; });

    this->bus.clear();

    EventDispatcher dispatcher(this->bus);
    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 0);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus allows unsubscribe during dispatch",
    "[integration][event_bus]"
) {
    int count = 0;
    EventBus::HandlerId id = 0;

    id = this->bus.subscribe<TestEvent>([&](const TestEvent&)
    {
        count++;
        this->bus.unsubscribe<TestEvent>(id);
    });

    EventDispatcher dispatcher(this->bus);

    dispatcher.post<TestEvent>({0});
    dispatcher.post<TestEvent>({0});
    dispatcher.process();
    dispatcher.process();

    REQUIRE(count == 1);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus allows subscribe during dispatch (new handler only next cycle)",
    "[integration][event_bus]")
{
    int count = 0;

    this->bus.subscribe<TestEvent>([&](const TestEvent &)
    {
        count++;
        this->bus.subscribe<TestEvent>([&](const TestEvent &) { count += 10; });
    });

    EventDispatcher dispatcher(this->bus);

    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 1);

    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 12);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus clear during dispatch does not cause UB",
    "[integration][event_bus]")
{
    int count = 0;

    this->bus.subscribe<TestEvent>([&](const TestEvent &)
    {
        count++;
        this->bus.clear();
    });

    this->bus.subscribe<TestEvent>([&](const TestEvent &){ count++; });

    EventDispatcher dispatcher(this->bus);

    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 2);

    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(count == 2);
}

TEST_CASE_METHOD(
    EventBusFixture,
    "EventBus supports deep reentrancy via dispatcher",
    "[integration][eventbus]")
{
    EventDispatcher dispatcher(this->bus);

    int depth = 0;

    this->bus.subscribe<TestEvent>([&](const TestEvent &)
    {
        depth++;
        if (depth < 5) dispatcher.post<TestEvent>({0});
    });

    dispatcher.post<TestEvent>({0});
    dispatcher.process();

    REQUIRE(depth == 1);

    dispatcher.process();
    REQUIRE(depth == 2);

    dispatcher.process();
    REQUIRE(depth == 3);

    dispatcher.process();
    REQUIRE(depth == 4);

    dispatcher.process();
    REQUIRE(depth == 5);
}