#include "game/flex_layout_handlers/GrowHandler.h"

#include "domain/components/FlexContainer.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/value_objects/FlexLayoutContext/FlexLayoutContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class GrowHandlerFixture
{
public:
    World world;
    EventBus bus;
    GrowHandler handler;
};

TEST_CASE("GrowHandler distributes free space proportionally", "[unit][grow_handler]")
{
    FlexContainer flex;
    World world;
    EventBus bus;
    GrowHandler handler;

    Entity fakeContainer{0};

    ChildLayoutInfo info1;
    info1.entity = Entity{1};
    info1.mainSize = 50.f;
    info1.grow = 1.f;

    ChildLayoutInfo info2;
    info2.entity = Entity{2};
    info2.mainSize = 50.f;
    info2.grow = 1.f;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = fakeContainer,
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{Position{0.f, 0.f}, Dimension2D{200.f, 100.f}},
        .childInfos = {info1, info2}
    };

    handler.layout(ctx);

    REQUIRE(ctx.childInfos[0].mainSize == Catch::Approx(100.f));
    REQUIRE(ctx.childInfos[1].mainSize == Catch::Approx(100.f));
    REQUIRE(ctx.freeSpace == Catch::Approx(0.f));
}

TEST_CASE("GrowHandler does not grow when no grow factor", "[unit][grow_handler]")
{
    FlexContainer flex;
    World world;
    EventBus bus;
    GrowHandler handler;

    ChildLayoutInfo info;
    info.entity = Entity{1};
    info.mainSize = 50.f;
    info.grow = 0.f;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{Position{0.f, 0.f}, Dimension2D{100.f, 100.f}},
        .childInfos = {info}
    };

    handler.layout(ctx);

    REQUIRE(ctx.childInfos[0].mainSize == Catch::Approx(50.f));
    REQUIRE(ctx.freeSpace == Catch::Approx(50.f));
}
