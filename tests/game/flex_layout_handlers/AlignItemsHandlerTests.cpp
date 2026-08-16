#include "game/flex_layout_handlers/AlignItemsHandler.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/value_objects/FlexLayoutContext/FlexLayoutContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class AlignItemsHandlerFixture
{
public:
    World world;
    EventBus bus;
    AlignItemsHandler handler;

    AlignItemsHandlerFixture()
    {
        auto& comp = world.components();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
    }

    Entity createChild(float width, float height)
    {
        Entity e = world.entities().create();
        world.components().add<TransformComponent>(e, TransformComponent{{0.f, 0.f}});
        world.components().add<UIRectComponent>(e, UIRectComponent{{width, height}});
        return e;
    }
};

TEST_CASE_METHOD(AlignItemsHandlerFixture, "AlignItemsHandler centers children cross-axis",
    "[unit][align_items_handler]")
{
    Entity child = createChild(50.f, 20.f);

    ChildLayoutInfo info;
    info.entity = child;
    info.crossSize = 20.f;

    FlexContainer flex;
    flex.align = FlexContainer::AlignItems::Center;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .innerRect  = Rectangle{Position{10.f, 10.f}, Dimension2D{100.f, 100.f}},
        .childInfos = {info},
        .isColumn   = false
    };

    handler.layout(ctx);

    const auto& transform = world.components().get<TransformComponent>(child);
    REQUIRE(transform.position.y == Catch::Approx(10.f + (100.f - 20.f) * 0.5f));
}

TEST_CASE_METHOD(AlignItemsHandlerFixture, "AlignItemsHandler stretches child cross-axis",
    "[unit][align_items_handler]")
{
    Entity child = createChild(50.f, 20.f);

    ChildLayoutInfo info;
    info.entity = child;
    info.crossSize = 20.f;

    FlexContainer flex;
    flex.align = FlexContainer::AlignItems::Stretch;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{Position{10.f, 10.f}, Dimension2D{100.f, 100.f}},
        .childInfos = {info},
        .isColumn   = false
    };

    handler.layout(ctx);

    const auto& uiRect = world.components().get<UIRectComponent>(child);
    REQUIRE(uiRect.size.height == Catch::Approx(100.f));
}
