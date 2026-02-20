#include "../../src/domain/include/EventDispatcher/EventDispatcher.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>
#include <vector>

struct DispatchEvent{ int value; };

struct EventDispatcherFixture
{
    EventBus bus;
    EventDispatcher dispatcher;

    EventDispatcherFixture() : bus(), dispatcher(bus) {}
};

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Dispatcher processes events in FIFO order",
    "[integration][event_dispatcher]")
{
    std::vector<int> order;

    this->bus.subscribe<DispatchEvent>(
        [&](const DispatchEvent &e) { order.push_back(e.value); });

    this->dispatcher.post<DispatchEvent>({1});
    this->dispatcher.post<DispatchEvent>({2});
    this->dispatcher.post<DispatchEvent>({3});

    this->dispatcher.process();

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 3);
}

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Dispatcher handles post from multiple threads",
    "[integration][event_dispatcher]")
{
    int count = 0;

    this->bus.subscribe<DispatchEvent>([&](const DispatchEvent &) { count++; });

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++)
    {
        threads.emplace_back([&]()
            { this->dispatcher.post<DispatchEvent>({0}); });
    }

    for (auto &t : threads) t.join();

    this->dispatcher.process();

    REQUIRE(count == 10);
}

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Events posted during process are deferred to next cycle",
    "[integration][event_dispatcher]")
{
    int count = 0;

    this->bus.subscribe<DispatchEvent>([&](const DispatchEvent &)
    {
        count++;
        this->dispatcher.post<DispatchEvent>({0});
    });

    this->dispatcher.post<DispatchEvent>({0});
    this->dispatcher.process();

    REQUIRE(count == 1);

    this->dispatcher.process();

    REQUIRE(count == 2);
}

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Dispatcher handles high volume of events",
    "[stress][event_dispatcher]"
) {
    int count = 0;

    this->bus.subscribe<DispatchEvent>([&](const DispatchEvent&) { count++; });

    const int N = 5000;

    for (int i = 0; i < N; ++i) this->dispatcher.post<DispatchEvent>({i});

    this->dispatcher.process();

    REQUIRE(count == N);
}

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Dispatcher handles concurrent high volume posting",
    "[stress][event_dispatcher]"
) {
    int count = 0;

    this->bus.subscribe<DispatchEvent>([&](const DispatchEvent&) { count++; });

    const int threadsCount = 8;
    const int perThread = 1000;

    std::vector<std::thread> threads;

    for (int t = 0; t < threadsCount; ++t)
    {
        threads.emplace_back([&]()
        { for (int i = 0; i < perThread; ++i) this->dispatcher.post<DispatchEvent>({i}); });
    }

    for (auto& t : threads) t.join();

    this->dispatcher.process();

    REQUIRE(count == threadsCount * perThread);
}

TEST_CASE_METHOD(
    EventDispatcherFixture,
    "Dispatcher remains stable when handlers post recursively",
    "[integration][event_dispatcher]"
) {
    int count = 0;

    this->bus.subscribe<DispatchEvent>([&](const DispatchEvent&)
    {
        count++;
        if (count < 3) this->dispatcher.post<DispatchEvent>({0});
    });

    this->dispatcher.post<DispatchEvent>({0});
    this->dispatcher.process();

    REQUIRE(count == 1);

    this->dispatcher.process();
    REQUIRE(count == 2);

    this->dispatcher.process();
    REQUIRE(count == 3);
}
