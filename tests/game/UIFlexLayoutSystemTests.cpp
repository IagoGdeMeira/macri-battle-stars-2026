#include "game/include/UIFlexLayoutSystem/UIFlexLayoutSystem.h"

#include "game/flex_layout_handlers/BoxModelHandler.h"
#include "game/flex_layout_handlers/GrowHandler.h"
#include "game/flex_layout_handlers/JustifyContentHandler.h"
#include "game/flex_layout_handlers/MainAxisHandler.h"
#include "game/flex_layout_handlers/AlignItemsHandler.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include "engine/include/CommandBuffer/CommandBuffer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class UIFlexLayoutSystemFixture
{
public:
    UIFlexLayoutSystemFixture() : ctx{this->world, this->bus, this->commandBuffer, 0.016f}
    {
        auto& comp = this->world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();

        this->system.addHandler(std::make_unique<BoxModelHandler>());
        this->system.addHandler(std::make_unique<GrowHandler>());
        this->system.addHandler(std::make_unique<JustifyContentHandler>());
        this->system.addHandler(std::make_unique<MainAxisHandler>());
        this->system.addHandler(std::make_unique<AlignItemsHandler>());
    }

    Entity createContainer(const Rectangle& rect, FlexDirection direction, JustifyContent justify, AlignItems align)
    {
        auto& comp = this->world.components();
        Entity e = this->world.entities().create();

        comp.add<TransformComponent>(e, TransformComponent{rect.position});
        comp.add<UILayoutMetricsComponent>(e, UILayoutMetricsComponent{rect.size});
        comp.add<FlexContainer>(e, FlexContainer{
            .direction  = direction,
            .justify    = justify,
            .align      = align,
            .gap        = 0.f
        });
        comp.add<LayoutDirtyComponent>(e, LayoutDirtyComponent{true});

        if (!comp.has<BoxModel>(e)) comp.add<BoxModel>(e, BoxModel{});
        return e;
    }

    Entity createChild(Entity parent, float width, float height, AlignItems alignSelf = AlignItems::FlexStart)
    {
        auto& comp = this->world.components();
        Entity child = this->world.entities().create();
        comp.add<TransformComponent>(child, TransformComponent{{0.f,0.f}});
        comp.add<UILayoutMetricsComponent>(child, UILayoutMetricsComponent{{width, height}});
        comp.add<FlexItem>(child, FlexItem{0.f, 0.f, -1.f, alignSelf});
        comp.add<ParentComponent>(child, ParentComponent{parent});
        return child;
    }

    World world;
    EventBus bus;
    CommandBuffer commandBuffer;
    UpdateContext ctx;
    UIFlexLayoutSystem system;
};

TEST_CASE_METHOD(UIFlexLayoutSystemFixture, "UIFlexLayoutSystem applies alignSelf on child", "[integration][ui_flex_layout_system]")
{
    Entity root = createContainer(
        Rectangle{Position{0.f, 0.f}, Dimension2D{800.f, 600.f}},
        FlexDirection::Column,
        JustifyContent::FlexStart,
        AlignItems::FlexStart
    );

    Entity child1 = createChild(root, 300.f, 30.f, AlignItems::FlexStart);
    Entity child2 = createChild(root, 100.f, 40.f, AlignItems::Center);

    this->system.update(ctx);

    auto& comp = this->world.components();
    const auto& local1 = comp.get<LocalTransform>(child1);
    const auto& local2 = comp.get<LocalTransform>(child2);

    REQUIRE(local1.position.x == Catch::Approx(0.f));
    REQUIRE(local2.position.x == Catch::Approx((800.f - 100.f) * 0.5f));
}
