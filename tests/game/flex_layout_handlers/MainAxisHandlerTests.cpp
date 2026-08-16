#include "game/flex_layout_handlers/MainAxisHandler.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/value_objects/FlexLayoutContext/FlexLayoutContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class MainAxisHandlerFixture
{
public:
    World world;
    EventBus bus;
    MainAxisHandler handler;

    MainAxisHandlerFixture()
    {
        auto& comp = world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
    }

    Entity createChild(float width, float height)
    {
        Entity e = world.entities().create();
        auto& comp = world.components();
        comp.add<TransformComponent>(e, TransformComponent{{0.f, 0.f}});
        comp.add<UIRectComponent>(e, UIRectComponent{{width, height}});
        return e;
    }
};

TEST_CASE_METHOD(MainAxisHandlerFixture, "MainAxisHandler positions children in row",
    "[unit][main_axis_handler]")
{
    Entity child1 = createChild(50.f, 20.f);
    Entity child2 = createChild(80.f, 30.f);

    ChildLayoutInfo info1;
    info1.entity = child1;
    info1.mainSize = 50.f;

    ChildLayoutInfo info2;
    info2.entity = child2;
    info2.mainSize = 80.f;

    FlexContainer flex;
    flex.gap = 10.f;

    FlexLayoutContext ctx{
        .world          = world,
        .container      = Entity{0},
        .flex           = flex,
        .children       = {child1, child2},
        .innerRect      = Rectangle{Position{10.f, 10.f}, Dimension2D{200.f, 100.f}},
        .childInfos     = {info1, info2},
        .isColumn       = false,
        .startOffset    = 0.f,
        .between        = 5.f
    };

    handler.layout(ctx);

    auto& comp = world.components();
    const auto& t1 = comp.get<TransformComponent>(child1);
    const auto& t2 = comp.get<TransformComponent>(child2);

    REQUIRE(t1.position.x == Catch::Approx(10.f));
    REQUIRE(t2.position.x == Catch::Approx(10.f + 50.f + 10.f + 5.f));

    const auto& r1 = comp.get<UIRectComponent>(child1);
    const auto& r2 = comp.get<UIRectComponent>(child2);
    REQUIRE(r1.size.width == Catch::Approx(50.f));
    REQUIRE(r2.size.width == Catch::Approx(80.f));
}
