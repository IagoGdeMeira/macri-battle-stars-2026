#include "engine/draw_batches/DrawRectangleBatch.h"

#include "StubRenderer.h"

#include "engine/draw_commands/DrawRectangleCommand.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("DrawRectangleBatch sorts by layer, zIndex, order", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawRectangleBatch batch;

    DrawRectangleCommand a, b, c, d;
    a.layer = 1; a.zIndex = 5;  a.order = 0; a.rect = {{0, 0}, {10, 10}};
    b.layer = 0; b.zIndex = 10; b.order = 2; b.rect = {{0, 0}, {10, 10}};
    c.layer = 0; c.zIndex = 10; c.order = 1; c.rect = {{0, 0}, {10, 10}};
    d.layer = 0; d.zIndex = 5;  d.order = 0; d.rect = {{0, 0}, {10, 10}};

    batch.add(a);
    batch.add(b);
    batch.add(c);
    batch.add(d);
    batch.submit(renderer);

    REQUIRE(renderer.rectangleCalls.size() == 4);
    REQUIRE(renderer.rectangleCalls[0].layer == 0);
    REQUIRE(renderer.rectangleCalls[0].zIndex == 5);
    REQUIRE(renderer.rectangleCalls[1].layer == 0);
    REQUIRE(renderer.rectangleCalls[1].zIndex == 10);
    REQUIRE(renderer.rectangleCalls[1].order == 1);
    REQUIRE(renderer.rectangleCalls[2].layer == 0);
    REQUIRE(renderer.rectangleCalls[2].zIndex == 10);
    REQUIRE(renderer.rectangleCalls[2].order == 2);
    REQUIRE(renderer.rectangleCalls[3].layer == 1);
}

TEST_CASE("DrawRectangleBatch clear empties commands", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawRectangleBatch batch;
    DrawRectangleCommand cmd;
    batch.add(cmd);
    batch.clear();
    batch.submit(renderer);
    REQUIRE(renderer.rectangleCalls.empty());
}
