#include "game/ui_widget_loaders/ShapeWidgetLoader.h"

#include "StubDataNode.h"
#include "StubFontFactory.h"
#include "StubTextureFactory.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/World/World.h"

#include "game/include/UIFactory/UIFactory.h"

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

class ShapeWidgetLoaderFixture
{
public:
    World world;
    StubFontFactory fontFactory;
    StubTextureFactory textureFactory;
    UIFactory factory;
    ShapeWidgetLoader loader;

    ShapeWidgetLoaderFixture() :
        factory(this->world, this->fontFactory, this->textureFactory),
        loader(this->factory)
    {
        auto& comp = this->world.components();
        comp.registerComponent<ChildrenComponent>();
        comp.registerComponent<CircleShapeComponent>();
        comp.registerComponent<LocalTransform>();
        comp.registerComponent<ParentComponent>();
        comp.registerComponent<RectangleShapeComponent>();
        comp.registerComponent<RenderComponent>();
        comp.registerComponent<TransformComponent>();
        comp.registerComponent<UILayoutMetricsComponent>();
    }

    std::unique_ptr<StubDataNode> makeRectangleNode(
        float width = 100.f, float height = 50.f,
        bool filled = true, bool withBorder = false,
        int zIndex = 0, int layer = 0
    ) {
        auto node = std::make_unique<StubDataNode>();
        node->setString("shape", "rectangle");
        node->setFloat("width", width);
        node->setFloat("height", height);
        node->setBool("filled", filled);
        node->setInt("zIndex", zIndex);
        node->setInt("layer", layer);

        auto fill = std::make_unique<StubDataNode>();
        fill->setInt("r", 255); fill->setInt("g", 0); fill->setInt("b", 0); fill->setInt("a", 255);
        node->setObject("fillColor", std::move(fill));

        if (withBorder)
        {
            auto border = std::make_unique<StubDataNode>();
            border->setInt("r", 0); border->setInt("g", 0); border->setInt("b", 0); border->setInt("a", 255);
            node->setObject("borderColor", std::move(border));
        }
        return node;
    }

    std::unique_ptr<StubDataNode> makeCircleNode(
        float radius = 30.f,
        bool filled = true, bool withBorder = false,
        int zIndex = 0, int layer = 0
    ) {
        auto node = std::make_unique<StubDataNode>();
        node->setString("shape", "circle");
        node->setFloat("radius", radius);
        node->setBool("filled", filled);
        node->setInt("zIndex", zIndex);
        node->setInt("layer", layer);

        auto fill = std::make_unique<StubDataNode>();
        fill->setInt("r", 0); fill->setInt("g", 255); fill->setInt("b", 0); fill->setInt("a", 255);
        node->setObject("fillColor", std::move(fill));

        if (withBorder)
        {
            auto border = std::make_unique<StubDataNode>();
            border->setInt("r", 0); border->setInt("g", 0); border->setInt("b", 0); border->setInt("a", 255);
            node->setObject("borderColor", std::move(border));
        }
        return node;
    }
};

TEST_CASE_METHOD(ShapeWidgetLoaderFixture, "ShapeWidgetLoader creates filled rectangle with border",
    "[integration][shape_widget_loader]")
{
    auto node = this->makeRectangleNode(200.f, 100.f, true, true, 5, 2);
    Entity shape = this->loader.load(*node);

    auto& comp = this->world.components();

    REQUIRE(comp.has<RectangleShapeComponent>(shape));
    REQUIRE(comp.has<RenderComponent>(shape));
    const auto& render = comp.get<RenderComponent>(shape);
    REQUIRE(render.zIndex == 5);
    REQUIRE(render.layer == 2);

    const auto& shapeComp = comp.get<RectangleShapeComponent>(shape);
    REQUIRE(shapeComp.rect.size.width == 200.f);
    REQUIRE(shapeComp.rect.size.height == 100.f);
    REQUIRE(shapeComp.filled == true);

    bool borderFound = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [entity, parent] : childrenView) if (parent.parent == shape)
    {
        borderFound = true;
        REQUIRE(comp.has<RectangleShapeComponent>(entity));
        const auto& borderRender = comp.get<RenderComponent>(entity);
        REQUIRE(borderRender.zIndex == 6);
        REQUIRE(borderRender.layer == 2);
        break;
    }
    REQUIRE(borderFound);
}

TEST_CASE_METHOD(ShapeWidgetLoaderFixture, "ShapeWidgetLoader creates filled circle with border",
    "[integration][shape_widget_loader]")
{
    auto node = this->makeCircleNode(40.f, true, true, 7, 1);
    Entity shape = this->loader.load(*node);

    auto& comp = this->world.components();

    REQUIRE(comp.has<CircleShapeComponent>(shape));
    REQUIRE(comp.has<RenderComponent>(shape));
    const auto& render = comp.get<RenderComponent>(shape);
    REQUIRE(render.zIndex == 7);
    REQUIRE(render.layer == 1);

    const auto& shapeComp = comp.get<CircleShapeComponent>(shape);
    REQUIRE(shapeComp.circle.radius == 40.f);
    REQUIRE(shapeComp.filled == true);

    bool borderFound = false;
    auto childrenView = View<ParentComponent>(comp);
    for (auto [entity, parent] : childrenView) if (parent.parent == shape)
    {
        borderFound = true;
        REQUIRE(comp.has<CircleShapeComponent>(entity));
        const auto& borderRender = comp.get<RenderComponent>(entity);
        REQUIRE(borderRender.zIndex == 8);
        REQUIRE(borderRender.layer == 1);
        break;
    }
    REQUIRE(borderFound);
}

TEST_CASE_METHOD(ShapeWidgetLoaderFixture, "ShapeWidgetLoader creates shape without border",
    "[integration][shape_widget_loader]")
{
    auto node = this->makeRectangleNode(150.f, 60.f, true, false, 3, 0);
    Entity shape = this->loader.load(*node);

    auto& comp = this->world.components();

    auto childrenView = View<ParentComponent>(comp);
    for (auto [entity, parent] : childrenView) if (parent.parent == shape) FAIL("Shape should not have border child");
}

TEST_CASE_METHOD(ShapeWidgetLoaderFixture, "ShapeWidgetLoader throws on unknown shape type",
    "[integration][shape_widget_loader]")
{
    auto node = std::make_unique<StubDataNode>();
    node->setString("shape", "triangle");

    REQUIRE_THROWS_AS(this->loader.load(*node), std::runtime_error);
}
