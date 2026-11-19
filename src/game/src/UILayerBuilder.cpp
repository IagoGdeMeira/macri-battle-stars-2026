#include "UILayerBuilder/UILayerBuilder.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/RenderComponent.h"
#include "domain/include/World/World.h"

#include "engine/include/DataNode/DataNode.h"
#include "engine/utils/DataUtils/DataUtils.h"

#include <algorithm>

void UILayerBuilder::build(Entity container, const DataNode& groupNode, const Position& baseOffset, const Dimension2D& baseSize) const
{
    if (!groupNode.has("layers")) return;

    for (auto& layer : groupNode.getArray("layers"))
    {
        std::string type = layer->getString("type", "texture");
        Position offset = this->resolveOffset(*layer, baseOffset);
        Dimension2D size = this->resolveSize(*layer, baseSize);

        AddData data{container, *layer, offset};
        if (type == "texture") this->addTexture(data, size);
        else if (type == "rectangle") this->addRectangle(data, size);
        else if (type == "circle") this->addCircle(data, size);
        else if (type == "polygon") this->addPolygon(data);
    }
}

Position UILayerBuilder::resolveOffset(const DataNode& layer, const Position& base) const
{
    if (!layer.has("position")) return base;
    auto posNode = layer.getObject("position");
    if (!posNode) return base;

    return Position{base.x + posNode->getFloat("x", 0.f), base.y + posNode->getFloat("y", 0.f)};
}

Dimension2D UILayerBuilder::resolveSize(const DataNode& layer, const Dimension2D& base) const
{
    if (!layer.has("size")) return base;
    auto sizeNode = layer.getObject("size");
    if (!sizeNode) return base;

    return Dimension2D{sizeNode->getFloat("width", base.width), sizeNode->getFloat("height", base.height)};
}

void UILayerBuilder::addTexture(AddData data, const Dimension2D& size) const
{
    std::string tex = data.layer.getString("texture", "");
    if (tex.empty()) return;

    Entity img = this->factory.createImage(tex, Rectangle{Position{0.f, 0.f}, size});
    this->factory.attachChild(data.container, img, data.offset);
    this->factory.world().components().get<RenderComponent>(img).zIndex = data.layer.getInt("zIndex", 0);
}

void UILayerBuilder::addRectangle(AddData data, const Dimension2D& size) const
{
    Color color = DataUtils::parseColor(*data.layer.getObject("color"), Color::WHITE());
    Entity rect = this->factory.createRectangleShape(UIFactory::ShapeParams{color, true}, Rectangle{Position{0.f, 0.f}, size});
    this->factory.attachChild(data.container, rect, data.offset);
    this->factory.world().components().get<RenderComponent>(rect).zIndex = data.layer.getInt("zIndex", 0);
}

void UILayerBuilder::addCircle(AddData data, const Dimension2D& size) const
{
    Color color = DataUtils::parseColor(*data.layer.getObject("color"), Color::WHITE());
    float radius = data.layer.getFloat("radius", std::min(size.width, size.height) * 0.5f);
    bool filled = data.layer.getBool("filled", true);
    Position center{data.offset.x + size.width * 0.5f, data.offset.y + size.height * 0.5f};

    Entity circle = this->factory.createCircleShape(UIFactory::ShapeParams{color, filled}, Circle{center, radius});
    this->factory.attachChild(data.container, circle, {0.f, 0.f});
    this->factory.world().components().get<RenderComponent>(circle).zIndex = data.layer.getInt("zIndex", 0);
}

void UILayerBuilder::addPolygon(AddData data) const
{
    if (!data.layer.has("vertices")) return;

    Polygon polygon;
    for (auto& vNode : data.layer.getArray("vertices"))
    {
        Position p;
        p.x = vNode->getFloat("x", 0.f);
        p.y = vNode->getFloat("y", 0.f);
        polygon.vertices.push_back(p);
    }
    if (polygon.vertices.size() < 3) return;


    Color color = DataUtils::parseColor(*data.layer.getObject("color"), Color::WHITE());
    bool filled = data.layer.getBool("filled", true);

    Entity poly = this->factory.createPolygonShape(UIFactory::ShapeParams{color, filled}, polygon);
    this->factory.attachChild(data.container, poly, data.offset);
    this->factory.world().components().get<RenderComponent>(poly).zIndex = data.layer.getInt("zIndex", 0);
}
