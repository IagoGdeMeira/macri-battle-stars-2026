#include "game/flex_layout_handlers/AlignItemsHandler.h"

#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

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
        auto& comp = this->world.components();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
    }

    Entity createChild(float width, float height)
    {
        Entity e = this->world.entities().create();
        auto& comp = this->world.components();
        comp.add<TransformComponent>(e, TransformComponent{{0.f, 0.f}});
        comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{{width, height}});
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
    flex.align = AlignItems::Center;

        FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{Position{10.f, 10.f}, Dimension2D{100.f, 100.f}},
        .childInfos = {info},
        .isColumn   = false
    };

    this->handler.layout(ctx);

    const auto& local = this->world.components().get<LocalTransform>(child);
    REQUIRE(local.position.y == Catch::Approx(10.f + (100.f - 20.f) * 0.5f));
}

TEST_CASE_METHOD(AlignItemsHandlerFixture, "AlignItemsHandler stretches child cross-axis",
    "[unit][align_items_handler]")
{
    Entity child = createChild(50.f, 20.f);

    ChildLayoutInfo info;
    info.entity = child;
    info.crossSize = 20.f;

    FlexContainer flex;
    flex.align = AlignItems::Stretch;

    FlexLayoutContext ctx{
        .world      = world,
        .container  = Entity{0},
        .flex       = flex,
        .children   = {},
        .innerRect  = Rectangle{Position{10.f, 10.f}, Dimension2D{100.f, 100.f}},
        .childInfos = {info},
        .isColumn   = false
    };

    this->handler.layout(ctx);

    const auto& layout = this->world.components().get<UILayoutMetricsComponent>(child);
    REQUIRE(layout.size.height == Catch::Approx(100.f));
}

TEST_CASE_METHOD(AlignItemsHandlerFixture, "AlignItemsHandler respects alignSelf override",
    "[unit][align_items_handler]")
{
    Entity child = createChild(50.f, 20.f);

    ChildLayoutInfo info;
    info.entity = child;
    info.crossSize = 20.f;

    FlexContainer flex;
    flex.align = AlignItems::FlexStart;

    auto& comp = world.components();
    comp.add<FlexItem>(child, FlexItem{
        .grow       = 0.f,
        .shrink     = 0.f,
        .basis      = -1.f,
        .alignSelf  = AlignItems::Center
    });

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

    const auto& local = this->world.components().get<LocalTransform>(child);
    REQUIRE(local.position.y == Catch::Approx(10.f + (100.f - 20.f) * 0.5f));
}
