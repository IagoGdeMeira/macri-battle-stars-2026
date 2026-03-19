#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"

#include "../../src/domain/include/World/World.h"

#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

TEST_CASE("CommandBuffer executes a single command",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    bool executed = false;

    buffer.create([&](World&) { executed = true; });
    buffer.flush(world);

    REQUIRE(executed);
}

TEST_CASE("CommandBuffer executes multiple commands in order",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    std::vector<int> order;

    buffer.create([&](World&) { order.push_back(1); });
    buffer.create([&](World&) { order.push_back(2); });
    buffer.create([&](World&) { order.push_back(3); });

    buffer.flush(world);

    REQUIRE(order.size() == 3);
    REQUIRE(order[0] == 1);
    REQUIRE(order[1] == 2);
    REQUIRE(order[2] == 3);
}

TEST_CASE("CommandBuffer clears commands after flush",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    int count = 0;

    buffer.create([&](World&) { count++; });

    buffer.flush(world);
    buffer.flush(world);

    REQUIRE(count == 1);
}

TEST_CASE("CommandBuffer can be reused after flush",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    int count = 0;

    buffer.create([&](World&) { count++; });
    buffer.flush(world);

    buffer.create([&](World&) { count++; });
    buffer.flush(world);

    REQUIRE(count == 2);
}

TEST_CASE("CommandBuffer passes correct World instance",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    World* received = nullptr;

    buffer.create([&](World& w) { received = &w; });
    buffer.flush(world);

    REQUIRE(received == &world);
}

TEST_CASE("CommandBuffer flush on empty buffer is a no-op",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    REQUIRE_NOTHROW(buffer.flush(world));
}

TEST_CASE("CommandBuffer propagates exceptions thrown by commands",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    int executedBeforeThrow = 0;

    buffer.create([&](World&) { executedBeforeThrow++; });
    buffer.create([&](World&) { throw std::runtime_error("boom"); });

    REQUIRE_THROWS_AS(buffer.flush(world), std::runtime_error);
    REQUIRE(executedBeforeThrow == 1);
}

TEST_CASE("CommandBuffer command can enqueue another command for next flush",
    "[unit][command_buffer]"
) {
    CommandBuffer buffer;
    World world;

    std::vector<int> order;

    buffer.create([&](World&)
    {
        order.push_back(1);
        buffer.create([&](World&) { order.push_back(2); });
    });

    buffer.flush(world);
    REQUIRE(order.size() == 1);
    REQUIRE(order[0] == 1);

    buffer.flush(world);
    REQUIRE(order.size() == 2);
    REQUIRE(order[1] == 2);
}
