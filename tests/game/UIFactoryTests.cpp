#include "../../src/game/include/UIFactory/UIFactory.h"

#include "../stubs/StubFont.h"
#include "../stubs/StubFontFactory.h"
#include "../stubs/StubTexture.h"
#include "../stubs/StubTextureFactory.h"

#include "../../src/domain/components/BoxModel.h"
#include "../../src/domain/components/FlexContainer.h"
#include "../../src/domain/components/FlexItem.h"
#include "../../src/domain/components/ParentComponent.h"
#include "../../src/domain/components/UIActionComponent.h"
#include "../../src/domain/components/UIFocusable.h"
#include "../../src/domain/components/UISpriteComponent.h"
#include "../../src/domain/components/UITextComponent.h"
#include "../../src/domain/components/UITransform.h"
#include "../../src/domain/include/World/World.h"

#include "../../src/game/include/IUIAction/IUIAction.h"

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
        auto& components = this->world.components();
        components.registerComponent<UITransform>();
        components.registerComponent<FlexContainer>();
        components.registerComponent<BoxModel>();
        components.registerComponent<UIFocusable>();
        components.registerComponent<ParentComponent>();
        components.registerComponent<FlexItem>();
        components.registerComponent<UITextComponent>();
        components.registerComponent<UISpriteComponent>();
        components.registerComponent<UIActionComponent>();
    }
};

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createPanel adds base UI components", "[unit][ui_factory]")
{
    Entity panel = this->factory.createPanel(Rectangle{Position{10.f, 20.f}, Dimension2D{100.f, 50.f}});

    auto& components = this->world.components();

    REQUIRE(components.has<UITransform>(panel));
    REQUIRE(components.has<FlexContainer>(panel));
    REQUIRE(components.has<BoxModel>(panel));

    const auto& transform = components.get<UITransform>(panel);
    REQUIRE(transform.rect.position.x == Catch::Approx(10.f));
    REQUIRE(transform.rect.position.y == Catch::Approx(20.f));
    REQUIRE(transform.rect.size.width == Catch::Approx(100.f));
    REQUIRE(transform.rect.size.height == Catch::Approx(50.f));

    const auto& box = components.get<BoxModel>(panel);
    REQUIRE(box.margin.left == Catch::Approx(0.f));
    REQUIRE(box.padding.left == Catch::Approx(8.f));
    REQUIRE(box.padding.top == Catch::Approx(8.f));
    REQUIRE(box.padding.right == Catch::Approx(8.f));
    REQUIRE(box.padding.bottom == Catch::Approx(8.f));
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createText builds a text entity",
    "[unit][ui_factory]"
) {
    Entity text = this->factory.createText("Play", 24.0f, Color::WHITE(), Position{ 5.0f, 6.0f });

    auto& components = this->world.components();

    REQUIRE(this->fontFactory.createFontCalls == 1);
    REQUIRE(this->fontFactory.lastPath == "assets/fonts/default.ttf");
    REQUIRE(components.has<UITransform>(text));
    REQUIRE(components.has<FlexItem>(text));
    REQUIRE(components.has<UITextComponent>(text));

    const auto& uiText = components.get<UITextComponent>(text);
    REQUIRE(uiText.text == "Play");
    REQUIRE(uiText.font == this->fontFactory.fontToReturn);
    REQUIRE(uiText.fontSize == Catch::Approx(24.0f));
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createImage builds a sprite entity",
    "[unit][ui_factory]"
) {
    Entity image = this->factory.createImage("assets/ui/icon.png", Rectangle{ Position{ 1.0f, 2.0f }, Dimension2D{ 32.0f, 48.0f } });

    auto& components = this->world.components();

    REQUIRE(this->textureFactory.createTextureCalls == 1);
    REQUIRE(this->textureFactory.lastPath == "assets/ui/icon.png");
    REQUIRE(components.has<UITransform>(image));
    REQUIRE(components.has<UISpriteComponent>(image));

    const auto& sprite = components.get<UISpriteComponent>(image);
    REQUIRE(sprite.texture == this->textureFactory.textureToReturn);
}

TEST_CASE_METHOD(UIFactoryFixture, "UIFactory createButton wires focus, text and action",
    "[unit][ui_factory]"
) {
    int executed = 0;
    auto action = std::make_shared<CountingAction>(executed);

    Entity button = this->factory.createButton("Start", Rectangle{Position{0.f, 0.f}, Dimension2D{200.f, 48.f}}, action);

    auto& components = this->world.components();

    std::optional<Entity> optEntity;
    auto view = View<ParentComponent>(components);
    for (auto [entity, parent] : view)
    {
        if (parent.parent != button) continue;
        
        optEntity = entity;
        break;        
    }
    REQUIRE(optEntity.has_value());
    Entity textEntity = *optEntity;

    REQUIRE(components.has<UITransform>(button));
    REQUIRE(components.has<FlexContainer>(button));
    REQUIRE(components.has<BoxModel>(button));
    REQUIRE(components.has<UIFocusable>(button));
    REQUIRE(components.has<UIActionComponent>(button));

    const auto& focusable = components.get<UIFocusable>(button);
    REQUIRE(focusable.canFocus);

    const auto& flex = components.get<FlexContainer>(button);
    REQUIRE(flex.direction == FlexContainer::FlexDirection::Row);
    REQUIRE(flex.justify == FlexContainer::JustifyContent::Center);
    REQUIRE(flex.align == FlexContainer::AlignItems::Center);

    REQUIRE(components.has<ParentComponent>(textEntity));
    REQUIRE(components.has<FlexItem>(textEntity));
    REQUIRE(components.has<UITextComponent>(textEntity));

    const auto& parentComp = components.get<ParentComponent>(textEntity);
    REQUIRE(parentComp.parent == button);

    const auto& text = components.get<UITextComponent>(textEntity);
    REQUIRE(text.text == "Start");

    components.get<UIActionComponent>(button).execute();
    REQUIRE(executed == 1);
}
