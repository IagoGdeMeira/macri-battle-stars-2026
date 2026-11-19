#include "game/include/UILayerBuilder/UILayerBuilder.h"

#include "StubDataNode.h"
#include "StubFontFactory.h"
#include "StubTextureFactory.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PolygonShapeComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/include/World/World.h"

#include "game/include/UIFactory/UIFactory.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <optional>
#include <vector>

class UILayerBuilderFixture
{
public:
    World world;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    UIFactory factory;
    UILayerBuilder builder;

    UILayerBuilderFixture() :
        factory(this->world, this->fontFactory, this->textureFactory),
        builder(this->factory)
    {
        auto& comp = this->world.components();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<PolygonShapeComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
        comp.registerComponent<UISpriteComponent>();
    }

    Entity createContainer() { return this->world.entities().create(); }

    std::optional<Entity> findFirstChild(Entity container)
    {
        auto& comp = this->world.components();
        auto view = View<ParentComponent>(comp);
        for (auto [e, p] : view) if (p.parent == container) return e;
        return std::nullopt;
    }

    int countChildren(Entity container)
    {
        auto& comp = this->world.components();
        int count = 0;
        auto view = View<ParentComponent>(comp);
        for (auto [e, p] : view) if (p.parent == container) ++count;
        return count;
    }

    std::unique_ptr<StubDataNode> makeColor(int r, int g, int b, int a = 255)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setInt("r", r); node->setInt("g", g); node->setInt("b", b); node->setInt("a", a);
        return node;
    }

    std::unique_ptr<StubDataNode> makeRectangleLayer(int zIndex = 0)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "rectangle");
        node->setObject("color", this->makeColor(255, 0, 0));
        node->setInt("zIndex", zIndex);
        return node;
    }

    std::unique_ptr<StubDataNode> makeTextureLayer(const std::string& tex, int zIndex = 0)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "texture");
        node->setString("texture", tex);
        node->setInt("zIndex", zIndex);
        return node;
    }

    std::unique_ptr<StubDataNode> makeCircleLayer(int zIndex = 0)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "circle");
        node->setObject("color", this->makeColor(0, 255, 0));
        node->setFloat("radius", 15.f);
        node->setBool("filled", true);
        node->setInt("zIndex", zIndex);
        return node;
    }

    std::unique_ptr<StubDataNode> makePolygonLayer(int zIndex = 0)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setString("type", "polygon");
        node->setObject("color", this->makeColor(0, 0, 255));
        node->setBool("filled", true);
        node->setInt("zIndex", zIndex);

        auto makeVert = [](float x, float y)
        {
            auto v = std::make_unique<StubDataNode>();
            v->setFloat("x", x);
            v->setFloat("y", y);
            return v;
        };

        std::vector<std::unique_ptr<DataNode>> verts;
        verts.push_back(makeVert(0.f, 0.f));
        verts.push_back(makeVert(20.f, 0.f));
        verts.push_back(makeVert(0.f, 10.f));
        node->setArray("vertices", std::move(verts));
        return node;
    }

    std::unique_ptr<StubDataNode> makeGroup(std::vector<std::unique_ptr<DataNode>> layers)
    {
        auto node = std::make_unique<StubDataNode>();
        node->setArray("layers", std::move(layers));
        return node;
    }
};

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates nothing for group without layers",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();
    auto group = std::make_unique<StubDataNode>();

    this->builder.build(container, *group, {0.f, 0.f}, {100.f, 50.f});

    REQUIRE(this->countChildren(container) == 0);
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates rectangle layer with zIndex and offset",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makeRectangleLayer(5));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {10.f, 20.f}, {200.f, 50.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    REQUIRE(comp.has<RectangleShapeComponent>(*child));
    const auto& shape = comp.get<RectangleShapeComponent>(*child);
    REQUIRE(shape.rect.size.width == Catch::Approx(200.f));
    REQUIRE(shape.rect.size.height == Catch::Approx(50.f));

    const auto& render = comp.get<RenderComponent>(*child);
    REQUIRE(render.zIndex == 5);

    const auto& local = comp.get<LocalTransform>(*child);
    REQUIRE(local.position.x == Catch::Approx(10.f));
    REQUIRE(local.position.y == Catch::Approx(20.f));
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder applies position override on top of base",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    auto layer = this->makeRectangleLayer(0);
    auto posNode = std::make_unique<StubDataNode>();
    posNode->setFloat("x", 5.f);
    posNode->setFloat("y", 7.f);
    layer->setObject("position", std::move(posNode));

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(std::move(layer));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {10.f, 20.f}, {200.f, 50.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    const auto& local = comp.get<LocalTransform>(*child);
    REQUIRE(local.position.x == Catch::Approx(15.f));
    REQUIRE(local.position.y == Catch::Approx(27.f));
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder applies size override",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    auto layer = this->makeRectangleLayer(0);
    auto sizeNode = std::make_unique<StubDataNode>();
    sizeNode->setFloat("width", 50.f);
    sizeNode->setFloat("height", 25.f);
    layer->setObject("size", std::move(sizeNode));

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(std::move(layer));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {0.f, 0.f}, {200.f, 100.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    const auto& shape = comp.get<RectangleShapeComponent>(*child);
    REQUIRE(shape.rect.size.width == Catch::Approx(50.f));
    REQUIRE(shape.rect.size.height == Catch::Approx(25.f));
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates texture layer",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makeTextureLayer("assets/test.png", 7));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {0.f, 0.f}, {100.f, 50.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    REQUIRE(comp.has<UISpriteComponent>(*child));
    const auto& render = comp.get<RenderComponent>(*child);
    REQUIRE(render.zIndex == 7);

    REQUIRE(this->textureFactory.createTextureCalls == 1);
    REQUIRE(this->textureFactory.lastPath == "assets/test.png");
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder skips texture with empty path",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makeTextureLayer("", 0));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {0.f, 0.f}, {100.f, 50.f});

    REQUIRE(this->countChildren(container) == 0);
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates circle layer",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makeCircleLayer(3));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {10.f, 20.f}, {100.f, 50.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    REQUIRE(comp.has<CircleShapeComponent>(*child));
    const auto& shape = comp.get<CircleShapeComponent>(*child);
    REQUIRE(shape.circle.radius == Catch::Approx(15.f));

    REQUIRE(shape.circle.position.x == Catch::Approx(60.f));
    REQUIRE(shape.circle.position.y == Catch::Approx(45.f));

    const auto& render = comp.get<RenderComponent>(*child);
    REQUIRE(render.zIndex == 3);
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates polygon layer",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makePolygonLayer(2));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {5.f, 5.f}, {100.f, 50.f});

    auto child = this->findFirstChild(container);
    REQUIRE(child.has_value());

    auto& comp = this->world.components();
    REQUIRE(comp.has<PolygonShapeComponent>(*child));
    const auto& shape = comp.get<PolygonShapeComponent>(*child);
    REQUIRE(shape.polygon.vertices.size() == 3);

    const auto& render = comp.get<RenderComponent>(*child);
    REQUIRE(render.zIndex == 2);

    const auto& local = comp.get<LocalTransform>(*child);
    REQUIRE(local.position.x == Catch::Approx(5.f));
    REQUIRE(local.position.y == Catch::Approx(5.f));
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder skips polygon with fewer than 3 vertices",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    auto layer = std::make_unique<StubDataNode>();
    layer->setString("type", "polygon");
    layer->setObject("color", this->makeColor(0, 0, 255));

    auto makeVert = [](float x, float y)
    {
        auto v = std::make_unique<StubDataNode>();
        v->setFloat("x", x);
        v->setFloat("y", y);
        return v;
    };

    std::vector<std::unique_ptr<DataNode>> verts;
    verts.push_back(makeVert(0.f, 0.f));
    verts.push_back(makeVert(10.f, 0.f));
    layer->setArray("vertices", std::move(verts));

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(std::move(layer));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {0.f, 0.f}, {100.f, 50.f});

    REQUIRE(this->countChildren(container) == 0);
}

TEST_CASE_METHOD(UILayerBuilderFixture, "UILayerBuilder creates multiple layers preserving type and zIndex",
    "[unit][ui_layer_builder]")
{
    Entity container = this->createContainer();

    std::vector<std::unique_ptr<DataNode>> layers;
    layers.push_back(this->makeRectangleLayer(10));
    layers.push_back(this->makeCircleLayer(20));
    layers.push_back(this->makePolygonLayer(30));
    auto group = this->makeGroup(std::move(layers));

    this->builder.build(container, *group, {0.f, 0.f}, {100.f, 50.f});

    REQUIRE(this->countChildren(container) == 3);

    int rectCount = 0, circleCount = 0, polyCount = 0;
    auto& comp = this->world.components();
    auto view = View<ParentComponent>(comp);
    for (auto [e, p] : view)
    {
        if (p.parent != container) continue;
        if (comp.has<RectangleShapeComponent>(e)) ++rectCount;
        if (comp.has<CircleShapeComponent>(e)) ++circleCount;
        if (comp.has<PolygonShapeComponent>(e)) ++polyCount;
    }

    REQUIRE(rectCount == 1);
    REQUIRE(circleCount == 1);
    REQUIRE(polyCount == 1);
}
