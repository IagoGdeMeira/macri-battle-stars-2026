#include "game/include/UILoader/UILoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubFontFactory.h"
#include "StubSceneManager.h"
#include "StubTextureFactory.h"

#include "domain/components/BoxModel.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIActionComponent.h"
#include "domain/components/UIFocusable.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/utils/DataUtils/DataUtils.h"

#include "game/include/UIFactory/UIFactory.h"
#include "game/include/UIActionFactory/UIActionFactory.h"
#include "game/ui_widget_loaders/HealthBarWidgetLoader.h"
#include "game/ui_widget_loaders/TimerWidgetLoader.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>

class UILoaderFixture
{
public:
    World world;
    EventBus bus;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    StubSceneManager sceneManager;
    StubDataParser parser;
    UIFactory factory;
    UIActionFactory actionFactory;
    UILoader loader;

    UILoaderFixture() :
        factory(this->world, this->fontFactory, this->textureFactory),
        actionFactory(UIActionFactory::Config{this->bus, this->sceneManager}),
        loader(this->parser, this->factory, this->actionFactory, this->fontFactory)
    {
        auto& comp = this->world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<HealthBarSegmentComponent>();
        comp.registerComponent<HealthBarTag>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<RoundTimerTag>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UIActionComponent>();
        comp.registerComponent<UIFocusable>();
        comp.registerComponent<UILayoutMetricsComponent>();
        comp.registerComponent<UISpriteComponent>();
        comp.registerComponent<UITextComponent>();

        this->loader.registerWidgetLoader("healthBar", std::make_unique<HealthBarWidgetLoader>(this->factory));
        this->loader.registerWidgetLoader("timer", std::make_unique<TimerWidgetLoader>(this->factory, this->fontFactory));
    }

    std::unique_ptr<StubDataNode> makeElementNode(
        const std::string& type, float x = 0.f, float y = 0.f,
        float width = 100.f, float height = 100.f
    ) {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", type);

        auto pos = std::make_unique<StubDataNode>();
        pos->setFloat("x", x);
        pos->setFloat("y", y);
        node->setObject("position", std::move(pos));

        auto size = std::make_unique<StubDataNode>();
        size->setFloat("width", width);
        size->setFloat("height", height);
        node->setObject("size", std::move(size));

        return node;
    }
};

TEST_CASE_METHOD(UILoaderFixture, "UILoader loads a layout with panel and text", "[integration][ui_loader]")
{
    auto panelNode = this->makeElementNode("panel", 10.f, 20.f, 200.f, 100.f);

    auto textNode = this->makeElementNode("text", 30.f, 40.f, 0.f, 0.f);
    textNode->setString("text", "Hello");
    textNode->setFloat("fontSize", 32.f);

    auto colorNode = std::make_unique<StubDataNode>();
    colorNode->setInt("r", 255);
    colorNode->setInt("g", 255);
    colorNode->setInt("b", 255);
    colorNode->setInt("a", 255);
    textNode->setObject("color", std::move(colorNode));

    panelNode->setArray("children", [&] {
        std::vector<std::unique_ptr<DataNode>> arr;
        arr.push_back(std::move(textNode));
        return arr;
    }());

    StubDataNode root;
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(panelNode));
    root.setArray("elements", std::move(elements));
    this->parser.registerNode("layout.json", std::make_unique<StubDataNode>(root));

    auto entities = this->loader.loadLayout("layout.json");

    REQUIRE(entities.size() == 1);
    Entity panel = entities[0];

    auto& comp = this->world.components();
    REQUIRE(comp.has<TransformComponent>(panel));
    REQUIRE(comp.has<UILayoutMetricsComponent>(panel));
    REQUIRE(comp.has<FlexContainer>(panel));
    REQUIRE(comp.has<BoxModel>(panel));
    REQUIRE(comp.has<LayoutDirtyComponent>(panel));

    const auto& transform = comp.get<TransformComponent>(panel);
    REQUIRE(transform.position.x == Catch::Approx(10.f));
    REQUIRE(transform.position.y == Catch::Approx(20.f));

    const auto& uiLayout = comp.get<UILayoutMetricsComponent>(panel);
    REQUIRE(uiLayout.size.width == Catch::Approx(200.f));
    REQUIRE(uiLayout.size.height == Catch::Approx(100.f));

    std::optional<Entity> childOpt;
    auto parentView = View<ParentComponent>(comp);
    for (auto [e, p] : parentView) if (p.parent == panel) { childOpt = e; break; }

    REQUIRE(childOpt.has_value());
    Entity textEntity = *childOpt;

    REQUIRE(comp.has<UITextComponent>(textEntity));
    const auto& text = comp.get<UITextComponent>(textEntity);
    REQUIRE(text.text == "Hello");
    REQUIRE(text.fontSize == Catch::Approx(32.f));
}

TEST_CASE_METHOD(UILoaderFixture, "UILoader loads a widget from a single object", "[integration][ui_loader]")
{
    auto panelNode = this->makeElementNode("panel", 0.f, 0.f, 300.f, 50.f);
    this->parser.registerNode("widget.json", std::move(panelNode));

    Entity widget = this->loader.loadWidget("widget.json");

    auto& comp = this->world.components();
    REQUIRE(comp.has<FlexContainer>(widget));
    REQUIRE(comp.has<TransformComponent>(widget));
    REQUIRE(comp.has<UILayoutMetricsComponent>(widget));
}

TEST_CASE_METHOD(UILoaderFixture, "UILoader applies style from stylesheet", "[integration][ui_loader]")
{
    StubDataNode styleNode;
    styleNode.setString("id", "primaryButton");
    auto flexNode = std::make_unique<StubDataNode>();
    flexNode->setInt("direction", static_cast<int>(FlexContainer::FlexDirection::Row));
    flexNode->setInt("justify", static_cast<int>(FlexContainer::JustifyContent::Center));
    flexNode->setInt("align", static_cast<int>(FlexContainer::AlignItems::Center));
    flexNode->setFloat("gap", 5.f);
    styleNode.setObject("flex", std::move(flexNode));

    StubDataNode stylesRoot;
    std::vector<std::unique_ptr<DataNode>> stylesArray;
    stylesArray.push_back(std::make_unique<StubDataNode>(styleNode));
    stylesRoot.setArray("styles", std::move(stylesArray));
    this->parser.registerNode("styles.json", std::make_unique<StubDataNode>(stylesRoot));

    this->loader.loadStyleSheet("styles.json");

    auto panelNode = this->makeElementNode("panel", 0.f, 0.f, 100.f, 100.f);
    panelNode->setString("style", "primaryButton");

    StubDataNode root;
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(panelNode));
    root.setArray("elements", std::move(elements));
    this->parser.registerNode("layout_with_style.json", std::make_unique<StubDataNode>(root));

    auto entities = this->loader.loadLayout("layout_with_style.json");

    REQUIRE(entities.size() == 1);
    Entity entity = entities[0];

    auto& comp = this->world.components();
    REQUIRE(comp.has<FlexContainer>(entity));
    const auto& flex = comp.get<FlexContainer>(entity);
    REQUIRE(flex.direction == FlexContainer::FlexDirection::Row);
    REQUIRE(flex.justify == FlexContainer::JustifyContent::Center);
    REQUIRE(flex.align == FlexContainer::AlignItems::Center);
    REQUIRE(flex.gap == Catch::Approx(5.f));
}

TEST_CASE_METHOD(UILoaderFixture, "UILoader attaches action to button", "[integration][ui_loader]")
{
    auto buttonNode = this->makeElementNode("button", 0.f, 0.f, 200.f, 50.f);
    buttonNode->setString("text", "Quit");
    buttonNode->setString("action", "Quit");

    StubDataNode root;
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(buttonNode));
    root.setArray("elements", std::move(elements));
    this->parser.registerNode("layout_action.json", std::make_unique<StubDataNode>(root));

    auto entities = this->loader.loadLayout("layout_action.json");

    REQUIRE(entities.size() == 1);
    Entity button = entities[0];

    auto& comp = this->world.components();
    REQUIRE(comp.has<UIActionComponent>(button));
    REQUIRE(comp.has<UIFocusable>(button));
}

TEST_CASE_METHOD(UILoaderFixture, "UILoader instantiates widget with parameters", "[integration][ui_loader]")
{
    auto widgetNode = this->makeElementNode("widget", 0.f, 0.f, 0.f, 0.f);
    widgetNode->setString("widgetType", "healthBar");
    this->parser.registerNode("widget_healthbar.json", std::move(widgetNode));

    UILoader::ParamMap params;
    params["playerId"] = "1";
    params["maxHealth"] = "300";
    params["currentHealth"] = "150";

    Entity widget = this->loader.instantiateWidget("widget_healthbar.json", params);

    auto& comp = this->world.components();
    REQUIRE(comp.has<HealthBarTag>(widget));
    const auto& tag = comp.get<HealthBarTag>(widget);
    REQUIRE(tag.playerId == 1);
    REQUIRE(tag.maxHealth == 300);
    REQUIRE(tag.currentHealth == 150);
}

TEST_CASE_METHOD(UILoaderFixture, "UILoader loads widget from external source", "[integration][ui_loader]")
{
    auto timerWidgetNode = this->makeElementNode("widget", 10.f, 20.f, 150.f, 50.f);
    timerWidgetNode->setString("widgetType", "timer");
    timerWidgetNode->setFloat("initialTime", 42.f);
    this->parser.registerNode("timer_widget_source.json", std::move(timerWidgetNode));

    auto sourceNode = this->makeElementNode("widget");
    sourceNode->setString("source", "timer_widget_source.json");

    StubDataNode root;
    std::vector<std::unique_ptr<DataNode>> elements;
    elements.push_back(std::move(sourceNode));
    root.setArray("elements", std::move(elements));
    this->parser.registerNode("layout_with_source.json", std::make_unique<StubDataNode>(root));

    auto entities = this->loader.loadLayout("layout_with_source.json");

    REQUIRE(entities.size() == 1);
    Entity entity = entities[0];

    auto& comp = this->world.components();
    REQUIRE(comp.has<RoundTimerTag>(entity));
    REQUIRE(comp.has<TransformComponent>(entity));
    REQUIRE(comp.has<UILayoutMetricsComponent>(entity));

    std::optional<Entity> textEntity;
    auto view = View<ParentComponent>(comp);
    for (auto [e, p] : view) if (p.parent == entity) { textEntity = e; break; }

    REQUIRE(textEntity.has_value());
    REQUIRE(comp.has<UITextComponent>(*textEntity));
    const auto& uiText = comp.get<UITextComponent>(*textEntity);
    REQUIRE(uiText.text == "42");
}
