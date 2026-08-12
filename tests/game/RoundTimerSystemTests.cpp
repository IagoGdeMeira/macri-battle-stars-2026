#include "game/include/RoundTimerSystem/RoundTimerSystem.h"

#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include "game/events/RoundTimeEvent.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <vector>

struct CollectedTime { float remaining; };

TEST_CASE("RoundTimerSystem emits event when second changes", "[unit][round_timer_system]")
{
    EventBus bus;
    World world;
    CommandBuffer cmd;
    UpdateContext ctx{world, bus, cmd, 0.016f};
    RoundTimerSystem system(bus, 10.f);

    std::vector<CollectedTime> times;
    bus.subscribe<RoundTimeEvent>([&](const RoundTimeEvent& e) { times.push_back({e.remainingSeconds}); });

    ctx.deltaTime = 1.f;
    system.update(ctx);
    REQUIRE(times.size() == 1);
    REQUIRE(times.back().remaining == Catch::Approx(9.0f).margin(0.01f));

    ctx.deltaTime = 0.5f;
    system.update(ctx);
    REQUIRE(times.size() == 2);
    REQUIRE(times.back().remaining == Catch::Approx(8.5f).margin(0.01f));

    ctx.deltaTime = 0.5f;
    system.update(ctx);
    REQUIRE(times.size() == 2);
}

TEST_CASE("RoundTimerSystem clamps at zero", "[unit][round_timer_system]")
{
    EventBus bus;
    World world;
    CommandBuffer cmd;
    UpdateContext ctx{world, bus, cmd, 0.016f};
    RoundTimerSystem system(bus, 1.f);

    std::vector<CollectedTime> times;
    bus.subscribe<RoundTimeEvent>([&](const RoundTimeEvent& e)
    { times.push_back({e.remainingSeconds}); });

    ctx.deltaTime = 1.f;
    system.update(ctx);
    REQUIRE(times.size() == 1);
    REQUIRE(times.back().remaining == Catch::Approx(0.f).margin(0.01f));
}
