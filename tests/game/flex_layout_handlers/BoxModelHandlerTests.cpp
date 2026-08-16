#include "game/flex_layout_handlers/BoxModelHandler.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/CommandBuffer/CommandBuffer.h"

#include "game/value_objects/FlexLayoutContext/FlexLayoutContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class BoxModelHandlerFixture
{
public:
    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    BoxModelHandler handler;

    BoxModelHandlerFixture()
    {
        auto& comp = world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
    }

    Entity createContainer(const Rectangle& rect, const AABB& padding = {0, 0, 0, 0})
    {
        auto& comp = world.components();
        Entity e = world.entities().create();

        comp.add<TransformComponent>(e, TransformComponent{rect.position});
        comp.add<UIRectComponent>(e, UIRectComponent{rect.size});
        comp.add<FlexContainer>(e, FlexContainer{});
        comp.add<BoxModel>(e, BoxModel{ .padding = padding });

        return e;
    }

    Entity createChild(
        Entity parent, const Dimension2D& size, const AABB& margin = {0, 0, 0, 0},
        float grow = 0.f, float shrink = 1.f, float basis = -1.f
    ) {
        auto& comp = world.components();
        Entity child = world.entities().create();
        comp.add<TransformComponent>(child, TransformComponent{{0.f,0.f}});
        comp.add<UIRectComponent>(child, UIRectComponent{size});
        comp.add<FlexItem>(child, FlexItem{grow, shrink, basis});
        comp.add<BoxModel>(child, BoxModel{ .margin = margin });
        comp.add<ParentComponent>(child, ParentComponent{parent});
        return child;
    }
};

TEST_CASE_METHOD(BoxModelHandlerFixture, "BoxModelHandler calculates inner rect and child info",
    "[unit][box_model_handler]")
{
    Entity container = createContainer(
        Rectangle{Position{10.f,20.f}, Dimension2D{300.f,200.f}},
        AABB{5.f, 6.f, 7.f, 8.f}
    );

    Entity child1 = createChild(container, {100.f, 50.f});
    Entity child2 = createChild(container, {150.f, 75.f}, {2.f, 3.f, 4.f, 5.f}, 1.f, 0.f, 120.f);

    auto& comp = world.components();
    FlexLayoutContext ctx{
        .world      = world,
        .container  = container,
        .flex       = comp.get<FlexContainer>(container),
        .children   = {child1, child2}
    };

    handler.layout(ctx);

    REQUIRE(ctx.innerRect.position.x == Catch::Approx(10.f + 5.f));
    REQUIRE(ctx.innerRect.position.y == Catch::Approx(20.f + 7.f));
    REQUIRE(ctx.innerRect.size.width == Catch::Approx(300.f - 5.f - 6.f));
    REQUIRE(ctx.innerRect.size.height == Catch::Approx(200.f - 7.f - 8.f));

    REQUIRE(ctx.childInfos.size() == 2);
    const auto& info1 = ctx.childInfos[0];
    const auto& info2 = ctx.childInfos[1];

    REQUIRE(info1.entity == child1);
    REQUIRE(info1.mainSize == Catch::Approx(100.f));
    REQUIRE(info1.crossSize == Catch::Approx(50.f));
    REQUIRE(info1.grow == 0.f);
    REQUIRE(info1.shrink == 1.f);

    REQUIRE(info2.entity == child2);
    REQUIRE(info2.mainSize == Catch::Approx(120.f));
    REQUIRE(info2.crossSize == Catch::Approx(75.f));
    REQUIRE(info2.grow == 1.f);
    REQUIRE(info2.shrink == 0.f);
    REQUIRE(info2.margin.left == Catch::Approx(2.f));
    REQUIRE(info2.margin.right == Catch::Approx(3.f));
    REQUIRE(info2.margin.top == Catch::Approx(4.f));
    REQUIRE(info2.margin.bottom == Catch::Approx(5.f));
}
