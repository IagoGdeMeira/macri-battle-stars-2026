#include "engine/draw_batches/DrawFontBatch.h"

#include "StubRenderer.h"

#include "engine/draw_commands/DrawFontCommand.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("DrawFontBatch sorts correctly", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawFontBatch batch;

    DrawFontCommand f1, f2;
    f1.layer = 0; f1.zIndex = 0; f1.order = 20;
    f2.layer = 0; f2.zIndex = 0; f2.order = 10;

    batch.add(f1);
    batch.add(f2);
    batch.submit(renderer);

    REQUIRE(renderer.fontCalls.size() == 2);
    REQUIRE(renderer.fontCalls[0].order == 10);
    REQUIRE(renderer.fontCalls[1].order == 20);
}

TEST_CASE("DrawFontBatch clear works", "[engine][draw_batch]")
{
    StubRenderer renderer;
    DrawFontBatch batch;
    batch.add({});
    batch.clear();
    batch.submit(renderer);
    REQUIRE(renderer.fontCalls.empty());
}
