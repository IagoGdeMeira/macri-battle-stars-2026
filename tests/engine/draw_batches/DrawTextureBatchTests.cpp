#include "engine/draw_batches/DrawTextureBatch.h"

#include "StubRenderer.h"

#include "engine/draw_commands/DrawTextureCommand.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("DrawTextureBatch sorts correctly", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawTextureBatch batch;

    DrawTextureCommand t1, t2, t3;
    t1.layer = 0; t1.zIndex = 5;  t1.order = 3;
    t2.layer = 0; t2.zIndex = 5;  t2.order = 1;
    t3.layer = 0; t3.zIndex = 10; t3.order = 0;

    batch.add(t1);
    batch.add(t2);
    batch.add(t3);
    batch.submit(renderer);

    REQUIRE(renderer.textureCalls.size() == 3);
    REQUIRE(renderer.textureCalls[0].zIndex == 5);
    REQUIRE(renderer.textureCalls[0].order == 1);
    REQUIRE(renderer.textureCalls[1].zIndex == 5);
    REQUIRE(renderer.textureCalls[1].order == 3);
    REQUIRE(renderer.textureCalls[2].zIndex == 10);
}

TEST_CASE("DrawTextureBatch clear works", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawTextureBatch batch;
    batch.add({});
    batch.clear();
    batch.submit(renderer);
    REQUIRE(renderer.textureCalls.empty());
}
