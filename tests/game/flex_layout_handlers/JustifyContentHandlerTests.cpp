#include "game/flex_layout_handlers/JustifyContentHandler.h"

#include "domain/components/FlexContainer.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/value_objects/FlexLayoutContext/FlexLayoutContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("JustifyContentHandler flex-start leaves free space at end", "[unit][justify_handler]")
{
    FlexContainer flex;
    flex.justify = FlexContainer::JustifyContent::FlexStart;
    World world;
    EventBus bus;
    JustifyContentHandler handler;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .innerRect  = Rectangle{},
        .childInfos = {ChildLayoutInfo{}, ChildLayoutInfo{}},
        .freeSpace  = 50.f
    };

    handler.layout(ctx);

    REQUIRE(ctx.startOffset == Catch::Approx(0.f));
    REQUIRE(ctx.between == Catch::Approx(0.f));
}

TEST_CASE("JustifyContentHandler center splits free space", "[unit][justify_handler]")
{
    FlexContainer flex;
    flex.justify = FlexContainer::JustifyContent::Center;
    World world;
    EventBus bus;
    JustifyContentHandler handler;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .innerRect  = Rectangle{},
        .childInfos = {ChildLayoutInfo{}, ChildLayoutInfo{}},
        .freeSpace  = 50.f
    };

    handler.layout(ctx);

    REQUIRE(ctx.startOffset == Catch::Approx(25.f));
    REQUIRE(ctx.between == Catch::Approx(0.f));
}

TEST_CASE("JustifyContentHandler space-between distributes between children", "[unit][justify_handler]")
{
    FlexContainer flex;
    flex.justify = FlexContainer::JustifyContent::SpaceBetween;
    World world;
    EventBus bus;
    JustifyContentHandler handler;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{},
        .childInfos = {ChildLayoutInfo{}, ChildLayoutInfo{}},
        .freeSpace  = 50.f
    };

    handler.layout(ctx);

    REQUIRE(ctx.startOffset == Catch::Approx(0.f));
    REQUIRE(ctx.between == Catch::Approx(50.f));
}
