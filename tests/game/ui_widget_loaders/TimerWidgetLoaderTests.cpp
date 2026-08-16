#include "game/ui_widget_loaders/TimerWidgetLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubFontFactory.h"
#include "StubSceneManager.h"
#include "StubTextureFactory.h"

#include "domain/components/BoxModel.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/include/UIFactory/UIFactory.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

class TimerWidgetLoaderFixture
{
public:
    World world;
    EventBus bus;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    UIFactory factory;
    TimerWidgetLoader loader;

    TimerWidgetLoaderFixture() :
        factory(this->world, this->fontFactory, this->textureFactory),
        loader(this->factory)
    {
        auto& comp = this->world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RoundTimerTag>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIRectComponent>();
        comp.registerComponent<UITextComponent>();
    }

    std::unique_ptr<StubDataNode> makeNode(
        float x = 0.f, float y = 0.f, float width = 100.f,
        float height = 40.f, float initialTime = 99.f
    ) {
        auto node = std::make_unique<StubDataNode>();
        node->setFloat("x", x);
        node->setFloat("y", y);
        node->setFloat("width", width);
        node->setFloat("height", height);
        node->setFloat("initialTime", initialTime);
        return node;
    }
};

TEST_CASE_METHOD(TimerWidgetLoaderFixture, "TimerWidgetLoader creates panel with RoundTimerTag and text child",
    "[integration][timer_widget_loader]"
) {
    auto node = this->makeNode(10.f, 20.f, 150.f, 50.f, 30.f);
    Entity panel = this->loader.load(*node, {});

    auto& comp = this->world.components();
    REQUIRE(comp.has<RoundTimerTag>(panel));

    std::optional<Entity> textEntity;
    auto view = View<ParentComponent>(comp);
    for (auto [e, parent] : view) if (parent.parent == panel) { textEntity = e; break; }

    REQUIRE(textEntity.has_value());
    Entity text = *textEntity;

    REQUIRE(comp.has<UITextComponent>(text));
    const auto& uiText = comp.get<UITextComponent>(text);
    REQUIRE(uiText.text == "30");
}
