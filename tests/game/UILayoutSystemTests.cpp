#include "../../src/game/include/UILayoutSystem/UILayoutSystem.h"

#include "../../src/domain/components/BoxModel.h"
#include "../../src/domain/components/FlexContainer.h"
#include "../../src/domain/components/FlexItem.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/engine/include/CommandBuffer/CommandBuffer.h"
#include "../../src/engine/include/EventBus/EventBus.h"
#include "../../src/engine/include/UpdateContext/UpdateContext.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class UILayoutSystemFixture
{
public:
    World world;

    UILayoutSystemFixture()
    {
        auto& components = this->world.components();
        components.registerComponent<UITransform>();
        components.registerComponent<FlexContainer>();
        components.registerComponent<ParentComponent>();
        components.registerComponent<BoxModel>();
        components.registerComponent<FlexItem>();
    }
};

TEST_CASE_METHOD(UILayoutSystemFixture, "UILayoutSystem positions children in a row with gap",
    "[unit][ui_layout_system]"
) {
    Entity container(0);

    auto& components = this->world.components();
    components.add<UITransform>(container, UITransform{});
    components.get<UITransform>(container).rect.position.x = 0.0f;
    components.get<UITransform>(container).rect.position.y = 0.0f;
    components.get<UITransform>(container).rect.size.width = 300.0f;
    components.get<UITransform>(container).rect.size.height = 100.0f;

    FlexContainer flex;
    flex.direction = FlexContainer::FlexDirection::Row;
    flex.gap = 10.0f;
    flex.justify = FlexContainer::JustifyContent::FlexStart;
    flex.align = FlexContainer::AlignItems::FlexStart;
    components.add<FlexContainer>(container, flex);

    Entity c1(1), c2(2), c3(3);

    UITransform t1; t1.rect.size.width = 50.0f; t1.rect.size.height = 50.0f;
    UITransform t2; t2.rect.size.width = 60.0f; t2.rect.size.height = 50.0f;
    UITransform t3; t3.rect.size.width = 70.0f; t3.rect.size.height = 50.0f;

    components.add<ParentComponent>(c1, ParentComponent{ container });
    components.add<UITransform>(c1, t1);

    components.add<ParentComponent>(c2, ParentComponent{ container });
    components.add<UITransform>(c2, t2);

    components.add<ParentComponent>(c3, ParentComponent{ container });
    components.add<UITransform>(c3, t3);

    EventBus eb;
    CommandBuffer cb;
    UpdateContext ctx{ this->world, eb, cb, 1.0f / 60.0f };

    UILayoutSystem system;
    system.update(ctx);

    auto& tr1 = components.get<UITransform>(c1);
    auto& tr2 = components.get<UITransform>(c2);
    auto& tr3 = components.get<UITransform>(c3);

    REQUIRE(tr1.rect.position.x == Catch::Approx(0.0f));
    REQUIRE(tr2.rect.position.x == Catch::Approx(60.0f));
    REQUIRE(tr3.rect.position.x == Catch::Approx(130.0f));
}

TEST_CASE_METHOD(UILayoutSystemFixture, "UILayoutSystem aligns children cross-axis when column and center align",
    "[unit][ui_layout_system]"
) {
    auto& components = this->world.components();
    Entity container(10);
    components.add<UITransform>(container, UITransform{});
    components.get<UITransform>(container).rect.position.x = 5.0f;
    components.get<UITransform>(container).rect.position.y = 5.0f;
    components.get<UITransform>(container).rect.size.width = 200.0f;
    components.get<UITransform>(container).rect.size.height = 300.0f;

    FlexContainer flex;
    flex.direction = FlexContainer::FlexDirection::Column;
    flex.gap = 5.0f;
    flex.align = FlexContainer::AlignItems::Center;
    components.add<FlexContainer>(container, flex);

    Entity c1(11), c2(12);
    UITransform t1; t1.rect.size.width = 40.0f; t1.rect.size.height = 20.0f;
    UITransform t2; t2.rect.size.width = 60.0f; t2.rect.size.height = 30.0f;

    components.add<ParentComponent>(c1, ParentComponent{ container });
    components.add<UITransform>(c1, t1);

    components.add<ParentComponent>(c2, ParentComponent{ container });
    components.add<UITransform>(c2, t2);

    EventBus eb;
    CommandBuffer cb;
    UpdateContext ctx{ this->world, eb, cb, 1.0f / 60.0f };

    UILayoutSystem system;
    system.update(ctx);

    float innerX = components.get<UITransform>(container).rect.position.x;
    float crossSize = components.get<UITransform>(container).rect.size.width;

    auto& tr1 = components.get<UITransform>(c1);
    auto& tr2 = components.get<UITransform>(c2);

    REQUIRE(tr1.rect.position.x == Catch::Approx(innerX + (crossSize - 40.0f) * 0.5f));
    REQUIRE(tr2.rect.position.x == Catch::Approx(innerX + (crossSize - 60.0f) * 0.5f));
}
