#include "game/ui_widget_loaders/HealthBarWidgetLoader.h"

#include "StubDataNode.h"
#include "StubDataParser.h"
#include "StubFontFactory.h"
#include "StubSceneManager.h"
#include "StubTextureFactory.h"

#include "IUIWidgetLoader/IParametrizedUIWidgetLoader.h"

#include "domain/components/BoxModel.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/FlexItem.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"

#include "game/include/UIFactory/UIFactory.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

class HealthBarWidgetLoaderFixture
{
public:
    World world;
    EventBus bus;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    StubDataParser parser;
    UIFactory factory;
    HealthBarWidgetLoader loader;

    HealthBarWidgetLoaderFixture() :
        factory(this->world, this->fontFactory, this->textureFactory),
        loader(this->factory, this->parser)
    {
        auto& comp = this->world.components();
        comp.registerComponent<BoxModel>();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<FlexContainer>();
        comp.registerComponent<FlexItem>();
        comp.registerComponent<HealthBarSegmentComponent>();
        comp.registerComponent<HealthBarTag>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
        comp.registerComponent<UISpriteComponent>();
        comp.registerComponent<UITextComponent>();

        StubDataNode colorsRoot;
        std::vector<std::unique_ptr<DataNode>> segmentsArray;

        auto segmentNode = std::make_unique<StubDataNode>();
        auto fillNode = std::make_unique<StubDataNode>();
        fillNode->setInt("r", 255); fillNode->setInt("g", 0); fillNode->setInt("b", 0); fillNode->setInt("a", 255);
        auto shadowNode = std::make_unique<StubDataNode>();
        shadowNode->setInt("r", 120); shadowNode->setInt("g", 0); shadowNode->setInt("b", 0); shadowNode->setInt("a", 255);
        segmentNode->setObject("fill", std::move(fillNode));
        segmentNode->setObject("shadow", std::move(shadowNode));
        segmentsArray.push_back(std::move(segmentNode));
        colorsRoot.setArray("segments", std::move(segmentsArray));
        this->parser.registerNode("assets/ui/health_bar_colors.json", std::make_unique<StubDataNode>(colorsRoot));
    }

    IParametrizedUIWidgetLoader::ParamMap makeParams(int playerId, int maxHealth, int currentHealth)
    {
        return {
            { "playerId", std::to_string(playerId) },
            { "maxHealth", std::to_string(maxHealth) },
            { "currentHealth", std::to_string(currentHealth) },
            { "characterName", "TestFighter" },
            { "avatarTexturePath", "" }
        };
    }

    std::unique_ptr<StubDataNode> makeNode(float width = 300.f, float height = 30.f)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setFloat("width", width);
        node->setFloat("height", height);
        node->setFloat("avatarSize", 0.f);
        return node;
    }
};

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader creates container with HealthBarTag and segments",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();
    auto params = this->makeParams(0, 300, 250);

    Entity bar = this->loader.load(*node, params);

    auto& comp = this->world.components();
    REQUIRE(comp.has<HealthBarTag>(bar));
    const auto& tag = comp.get<HealthBarTag>(bar);
    REQUIRE(tag.playerId == 0);
    REQUIRE(tag.maxHealth == 300);
    REQUIRE(tag.currentHealth == 250);

    std::vector<Entity> children;
    auto view = View<ParentComponent>(comp);
    for (auto [e, parent] : view) if (parent.parent == bar) children.push_back(e);

    REQUIRE(children.size() == 8);

    int segmentCount = 0;
    for (Entity child : children) if (comp.has<HealthBarSegmentComponent>(child))
    {
        ++segmentCount;
        const auto& seg = comp.get<HealthBarSegmentComponent>(child);
        REQUIRE(seg.maxHP == 100.f);
        REQUIRE(seg.maxWidth == Catch::Approx(300.f));
    }

    REQUIRE(segmentCount == 6);
}

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader handles zero health with full width background",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();
    auto params = this->makeParams(0, 100, 0);

    this->loader.load(*node, params);

    auto& comp = this->world.components();
    auto view = View<HealthBarSegmentComponent>(comp);
    for (auto [e, seg] : view)
    {
        auto& layout = comp.get<UILayoutMetricsComponent>(e);
        REQUIRE(layout.size.width == 0.f);
    }
}
