#include "engine/draw_batches/DrawCircleBatch.h"

#include "StubRenderer.h"

#include "engine/draw_commands/DrawCircleCommand.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("DrawCircleBatch sorts by layer, zIndex, order", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawCircleBatch batch;

    DrawCircleCommand x, y, z;
    x.layer = 2; x.zIndex = 0;  x.order = 0;
    y.layer = 1; y.zIndex = 20; y.order = 2;
    z.layer = 1; z.zIndex = 10; z.order = 5;

    batch.add(x);
    batch.add(y);
    batch.add(z);
    batch.submit(renderer);

    REQUIRE(renderer.circleCalls.size() == 3);
    REQUIRE(renderer.circleCalls[0].layer == 1);
    REQUIRE(renderer.circleCalls[0].zIndex == 10);
    REQUIRE(renderer.circleCalls[0].order == 5);
    REQUIRE(renderer.circleCalls[1].layer == 1);
    REQUIRE(renderer.circleCalls[1].zIndex == 20);
    REQUIRE(renderer.circleCalls[1].order == 2);
    REQUIRE(renderer.circleCalls[2].layer == 2);
}

TEST_CASE("DrawCircleBatch clear works", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawCircleBatch batch;
    batch.add({});
    batch.clear();
    batch.submit(renderer);
    REQUIRE(renderer.circleCalls.empty());
}
