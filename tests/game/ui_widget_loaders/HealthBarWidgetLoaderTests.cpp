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
#include "domain/components/HealthBarEndCapComponent.h"
#include "domain/components/HealthBarLayoutComponent.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/LayoutDirtyComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PolygonShapeComponent.h"
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
        comp.registerComponent<HealthBarEndCapComponent>();
        comp.registerComponent<HealthBarLayoutComponent>();
        comp.registerComponent<HealthBarSegmentComponent>();
        comp.registerComponent<HealthBarTag>();
        comp.registerComponent<LayoutDirtyComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<PolygonShapeComponent>();
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

    std::unique_ptr<StubDataNode> makeLayeredNode(float width = 300.f, float height = 30.f)
    {
        auto node = this->makeNode(width, height);

        auto barNode = std::make_unique<StubDataNode>();
        std::vector<std::unique_ptr<DataNode>> layers;

        auto bgLayer = std::make_unique<StubDataNode>();
        bgLayer->setString("type", "rectangle");
        auto bgColor = std::make_unique<StubDataNode>();
        bgColor->setInt("r", 60); bgColor->setInt("g", 60); bgColor->setInt("b", 60); bgColor->setInt("a", 255);
        bgLayer->setObject("color", std::move(bgColor));
        bgLayer->setInt("zIndex", 0);
        layers.push_back(std::move(bgLayer));

        barNode->setArray("layers", std::move(layers));
        node->setObject("bar", std::move(barNode));

        return node;
    }
};

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader creates container with HealthBarTag and segments",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeLayeredNode();
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

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader creates end cap when configured",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();
    node->setString("direction", "left");
    node->setFloat("barHeight", 30.f);

    auto capNode = std::make_unique<StubDataNode>();
    capNode->setFloat("width", 10.f);
    std::vector<std::unique_ptr<DataNode>> verts;
    auto makeVert = [](float x, float y)
    {
        auto v = std::make_unique<StubDataNode>();
        v->setFloat("x", x);
        v->setFloat("y", y);
        return v;
    };
    verts.push_back(makeVert(0.f, 0.f));
    verts.push_back(makeVert(10.f, 0.f));
    verts.push_back(makeVert(0.f, 30.f));
    capNode->setArray("vertices", std::move(verts));
    node->setObject("endCap", std::move(capNode));

    auto params = this->makeParams(0, 300, 150);
    Entity bar = this->loader.load(*node, params);

    auto& comp = this->world.components();

    REQUIRE(comp.has<HealthBarLayoutComponent>(bar));
    const auto& layout = comp.get<HealthBarLayoutComponent>(bar);
    REQUIRE(layout.flip == false);
    REQUIRE(layout.barOffsetX == Catch::Approx(0.f));
    REQUIRE(layout.barHeight == Catch::Approx(30.f));
    REQUIRE(layout.capWidth == Catch::Approx(10.f));

    bool capFound = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [e, parent] : childrenView) if (parent.parent == bar && comp.has<HealthBarEndCapComponent>(e))
    {
        capFound = true;
        REQUIRE(comp.has<PolygonShapeComponent>(e));
        const auto& poly = comp.get<PolygonShapeComponent>(e);
        REQUIRE(poly.polygon.vertices.size() == 3);
        break;
    }
    REQUIRE(capFound);
}

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader supports polygon layers",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();

    auto barNode = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> layers;

    auto polyLayer = std::make_unique<StubDataNode>();
    polyLayer->setString("type", "polygon");
    polyLayer->setInt("zIndex", 150);

    std::vector<std::unique_ptr<DataNode>> verts;
    auto makeVert = [](float x, float y)
    {
        auto v = std::make_unique<StubDataNode>();
        v->setFloat("x", x);
        v->setFloat("y", y);
        return v;
    };
    verts.push_back(makeVert(0.f, 0.f));
    verts.push_back(makeVert(20.f, 0.f));
    verts.push_back(makeVert(0.f, 10.f));
    polyLayer->setArray("vertices", std::move(verts));

    auto colorNode = std::make_unique<StubDataNode>();
    colorNode->setInt("r", 255); colorNode->setInt("g", 0); colorNode->setInt("b", 0); colorNode->setInt("a", 255);
    polyLayer->setObject("color", std::move(colorNode));

    layers.push_back(std::move(polyLayer));
    barNode->setArray("layers", std::move(layers));
    node->setObject("bar", std::move(barNode));

    auto params = this->makeParams(0, 100, 100);
    Entity bar = this->loader.load(*node, params);

    auto& comp = this->world.components();

    bool found = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [e, parent] : childrenView)
    {
        if (parent.parent != bar) continue;
        if (!comp.has<PolygonShapeComponent>(e)) continue;

        found = true;
        const auto& poly = comp.get<PolygonShapeComponent>(e);
        REQUIRE(poly.polygon.vertices.size() == 3);
        const auto& render = comp.get<RenderComponent>(e);
        REQUIRE(render.zIndex == 150);
        break;
    }
    REQUIRE(found);
}

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader supports texture layers in bar",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();

    auto barNode = std::make_unique<StubDataNode>();
    std::vector<std::unique_ptr<DataNode>> layers;

    auto texLayer = std::make_unique<StubDataNode>();
    texLayer->setString("type", "texture");
    texLayer->setString("texture", "assets/ui/frame.png");
    texLayer->setInt("zIndex", 200);
    layers.push_back(std::move(texLayer));

    barNode->setArray("layers", std::move(layers));
    node->setObject("bar", std::move(barNode));

    auto params = this->makeParams(0, 100, 100);
    Entity bar = this->loader.load(*node, params);

    auto& comp = this->world.components();

    bool found = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [e, parent] : childrenView)
    {
        if (parent.parent != bar) continue;
        if (!comp.has<UISpriteComponent>(e)) continue;

        const auto& render = comp.get<RenderComponent>(e);
        if (render.zIndex == 200) { found = true; break; }
    }
    REQUIRE(found);

    REQUIRE(this->textureFactory.createTextureCalls >= 1);
    REQUIRE(this->textureFactory.lastPath == "assets/ui/frame.png");
}

TEST_CASE_METHOD(HealthBarWidgetLoaderFixture, "HealthBarWidgetLoader creates avatar layers and photo",
    "[integration][health_bar_widget_loader]"
) {
    auto node = this->makeNode();
    node->setFloat("avatarSize", 60.f);

    auto avatarNode = std::make_unique<StubDataNode>();

    auto photoNode = std::make_unique<StubDataNode>();
    photoNode->setFloat("scale", 0.7f);
    photoNode->setInt("zIndex", 65);
    avatarNode->setObject("photo", std::move(photoNode));

    std::vector<std::unique_ptr<DataNode>> layers;
    auto bgLayer = std::make_unique<StubDataNode>();
    bgLayer->setString("type", "texture");
    bgLayer->setString("texture", "assets/ui/avatar_bg.png");
    bgLayer->setInt("zIndex", 50);
    layers.push_back(std::move(bgLayer));
    avatarNode->setArray("layers", std::move(layers));

    node->setObject("avatar", std::move(avatarNode));

    auto params = this->makeParams(0, 100, 100);
    params["avatarTexturePath"] = "assets/ui/avatar.png";

    Entity bar = this->loader.load(*node, params);

    auto& comp = this->world.components();

    bool bgFound = false;
    bool photoFound = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [e, parent] : childrenView)
    {
        if (parent.parent != bar) continue;
        if (!comp.has<UISpriteComponent>(e)) continue;

        const auto& render = comp.get<RenderComponent>(e);
        if (render.zIndex == 50) bgFound = true;
        if (render.zIndex == 65) photoFound = true;
    }

    REQUIRE(bgFound);
    REQUIRE(photoFound);
    REQUIRE(this->textureFactory.createTextureCalls >= 2);
}
