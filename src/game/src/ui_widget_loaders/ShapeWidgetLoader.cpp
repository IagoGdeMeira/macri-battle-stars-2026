#include "ShapeWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"

#include "engine/utils/DataUtils/DataUtils.h"

#include <stdexcept>

ShapeWidgetLoader::ShapeWidgetLoader(UIFactory& factory) : factory(factory) {}

Entity ShapeWidgetLoader::load(const DataNode& node)
{
    std::string shape = node.getString("shape", "rectangle");
    Color fillColor = Color::WHITE();
    Color borderColor = Color::TRANSPARENT();
    bool filled = node.getBool("filled", true);
    int zIndex = node.getInt("zIndex", 0);
    int layer = node.getInt("layer", 0);

    if (node.has("fillColor")) fillColor = DataUtils::parseColor(*node.getObject("fillColor"), Color::WHITE());
    if (node.has("borderColor")) borderColor = DataUtils::parseColor(*node.getObject("borderColor"), Color::TRANSPARENT());

    auto& comp = this->factory.world().components();

    if (shape == "rectangle")
    {
        Rectangle rect;
        if (node.has("size")) rect = DataUtils::parseRect(node, {{0.f, 0.f}, {100.f, 100.f}});
        else
        {
            float width = node.getFloat("width", 100.f);
            float height = node.getFloat("height", 100.f);
            rect = Rectangle{Position{0.f, 0.f}, Dimension2D{width, height}};
        }

        Entity entity = this->factory.createRectangleShape(UIFactory::ShapeParams{fillColor, filled}, rect);

        if (borderColor.a > 0)
        {
            Entity border = this->factory.createRectangleShape(UIFactory::ShapeParams{borderColor, false}, rect);
            this->factory.attachChild(entity, border, {0.f, 0.f});
            auto& borderRender = comp.get<RenderComponent>(border);
            borderRender.zIndex = zIndex + 1;
            borderRender.layer = layer;
        }
        this->applyRenderComponent(entity, zIndex, layer);
        return entity;
    }
    if (shape == "circle")
    {
        Circle circle = DataUtils::parseCircle(node, {{0.f, 0.f}, 10.f});
        Entity entity = this->factory.createCircleShape(UIFactory::ShapeParams{fillColor, filled}, circle);

        if (borderColor.a > 0)
        {
            Entity border = this->factory.createCircleShape(UIFactory::ShapeParams{borderColor, false}, circle);
            this->factory.attachChild(entity, border, {0.f, 0.f});
            auto& borderRender = comp.get<RenderComponent>(border);
            borderRender.zIndex = zIndex + 1;
            borderRender.layer = layer;
        }
        this->applyRenderComponent(entity, zIndex, layer);
        return entity;
    }
    throw std::runtime_error("ShapeWidgetLoader: unknown shape type");
}

void ShapeWidgetLoader::applyRenderComponent(Entity entity, int zIndex, int layer)
{
    auto& comp = this->factory.world().components();
    if (comp.has<RenderComponent>(entity))
    {
        comp.get<RenderComponent>(entity).zIndex = zIndex;
        comp.get<RenderComponent>(entity).layer = layer;
    }
}
