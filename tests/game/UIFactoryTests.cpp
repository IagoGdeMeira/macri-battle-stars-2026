#include "game/include/UIFactory/UIFactory.h"

#include "StubFont.h"
#include "StubFontFactory.h"
#include "StubTexture.h"
#include "StubTextureFactory.h"

#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/FlexEnums/FlexEnums.h"

#include "game/include/IUIAction/IUIAction.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <optional>

class CountingAction : public IUIAction
{
public:
    explicit CountingAction(int& c) : count(c) {}
    void execute() const override { ++this->count; }
private:
    int& count;
};

class UIFactoryFixture
{
public:
    World world;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    UIFactory factory;

    UIFactoryFixture() : factory(this->world, this->fontFactory, this->textureFactory)
    {
        auto& comp = this->world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIActionComponent>();
        comp.registerComponent<UIFocusable>();
        comp.registerComponent<UILayoutMetricsComponent>();
        comp.registerComponent<UISpriteComponent>();
        comp.registerComponent<UITextComponent>();
    }
};

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createPanel adds base UI components", "[unit][ui_factory]")
{
    Entity panel = this->factory.createPanel(Rectangle{Position{10.f, 20.f}, Dimension2D{100.f, 50.f}});

    auto& comp = this->world.components();

    REQUIRE(comp.has<TransformComponent>(panel));
    REQUIRE(comp.has<UILayoutMetricsComponent>(panel));
    REQUIRE(comp.has<FlexContainer>(panel));
    REQUIRE(comp.has<BoxModel>(panel));

    const auto& transform = comp.get<TransformComponent>(panel);
    REQUIRE(transform.position.x == Catch::Approx(10.f));
    REQUIRE(transform.position.y == Catch::Approx(20.f));

    const auto& layout = comp.get<UILayoutMetricsComponent>(panel);
    REQUIRE(layout.size.width == Catch::Approx(100.f));
    REQUIRE(layout.size.height == Catch::Approx(50.f));

    const auto& box = comp.get<BoxModel>(panel);
    REQUIRE(box.margin.left == Catch::Approx(0.f));
    REQUIRE(box.padding.left == Catch::Approx(8.f));
    REQUIRE(box.padding.top == Catch::Approx(8.f));
    REQUIRE(box.padding.right == Catch::Approx(8.f));
    REQUIRE(box.padding.bottom == Catch::Approx(8.f));
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createText builds a text entity", "[unit][ui_factory]")
{
    UIFactory::TextParams params{"Play", 24.f, Color::WHITE(), Position{5.f, 6.f}};
    Entity text = this->factory.createText(params);

    auto& comp = this->world.components();

    REQUIRE(this->fontFactory.createFontCalls == 1);
    REQUIRE(this->fontFactory.lastPath == "assets/fonts/default.ttf");
    REQUIRE(comp.has<TransformComponent>(text));
    REQUIRE(comp.has<UILayoutMetricsComponent>(text));
    REQUIRE(comp.has<FlexItem>(text));
    REQUIRE(comp.has<UITextComponent>(text));

    const auto& uiText = comp.get<UITextComponent>(text);
    REQUIRE(uiText.text == "Play");
    REQUIRE(uiText.font == this->fontFactory.fontToReturn);
    REQUIRE(uiText.fontSize == Catch::Approx(24.f));
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createImage builds a sprite entity", "[unit][ui_factory]")
{
    Entity image = this->factory.createImage("assets/ui/icon.png", Rectangle{Position{1.f, 2.f}, Dimension2D{32.f, 48.f}});

    auto& comp = this->world.components();

    REQUIRE(this->textureFactory.createTextureCalls == 1);
    REQUIRE(this->textureFactory.lastPath == "assets/ui/icon.png");
    REQUIRE(comp.has<TransformComponent>(image));
    REQUIRE(comp.has<UILayoutMetricsComponent>(image));
    REQUIRE(comp.has<UISpriteComponent>(image));

    const auto& sprite = comp.get<UISpriteComponent>(image);
    REQUIRE(sprite.texture != nullptr);
    REQUIRE(dynamic_cast<StubTexture*>(sprite.texture.get()) != nullptr);
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createButton wires focus, text and action", "[unit][ui_factory]")
{
    int executed = 0;
    auto action = std::make_shared<CountingAction>(executed);

    Entity button = this->factory.createButton("Start", Rectangle{Position{0.f, 0.f}, Dimension2D{200.f, 48.f}}, action);

    auto& comp = this->world.components();

    std::optional<Entity> optEntity;
    auto view = View<ParentComponent>(comp);
    for (auto [entity, parent] : view)
    {
        if (parent.parent != button) continue;
        optEntity = entity;
        break;
    }
    REQUIRE(optEntity.has_value());
    Entity textEntity = *optEntity;

    REQUIRE(comp.has<TransformComponent>(button));
    REQUIRE(comp.has<UILayoutMetricsComponent>(button));
    REQUIRE(comp.has<FlexContainer>(button));
    REQUIRE(comp.has<BoxModel>(button));
    REQUIRE(comp.has<UIFocusable>(button));
    REQUIRE(comp.has<UIActionComponent>(button));

    const auto& focusable = comp.get<UIFocusable>(button);
    REQUIRE(focusable.canFocus);

    const auto& flex = comp.get<FlexContainer>(button);
    REQUIRE(flex.direction == FlexDirection::Row);
    REQUIRE(flex.justify == JustifyContent::Center);
    REQUIRE(flex.align == AlignItems::Center);

    REQUIRE(comp.has<ParentComponent>(textEntity));
    REQUIRE(comp.has<FlexItem>(textEntity));
    REQUIRE(comp.has<UITextComponent>(textEntity));

    const auto& parentComp = comp.get<ParentComponent>(textEntity);
    REQUIRE(parentComp.parent == button);

    const auto& text = comp.get<UITextComponent>(textEntity);
    REQUIRE(text.text == "Start");

    comp.get<UIActionComponent>(button).execute();
    REQUIRE(executed == 1);
}
