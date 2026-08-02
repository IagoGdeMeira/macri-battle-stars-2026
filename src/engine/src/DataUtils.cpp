#include "DataUtils/DataUtils.h"

Position DataUtils::parsePosition(const DataNode& node, const Position& defaultValue)
{
    if (!node.has("x") || !node.has("y")) return defaultValue;
    
    Position pos;
    pos.x = node.getFloat("x", defaultValue.x);
    pos.y = node.getFloat("y", defaultValue.y);
    return pos;
}

Dimension2D DataUtils::parseSize(const DataNode& node, const Dimension2D& defaultValue)
{
    if (!node.has("width") || !node.has("height")) return defaultValue;
    
    Dimension2D size;
    size.width  = node.getFloat("width",  defaultValue.width);
    size.height = node.getFloat("height", defaultValue.height);
    return size;
}

Rectangle DataUtils::parseRect(const DataNode& node, const Rectangle& defaultValue)
{
    if (!node.has("position") || !node.has("size")) return defaultValue;

    auto posNode    = node.getObject("position");
    auto sizeNode   = node.getObject("size");
    
    if (!posNode || !sizeNode) return defaultValue;

    Rectangle rect;
    rect.position   = DataUtils::parsePosition(*posNode, defaultValue.position);
    rect.size       = DataUtils::parseSize(*sizeNode, defaultValue.size);

    return rect;
}

Circle DataUtils::parseCircle(const DataNode& node, const Circle& defaultValue)
{
    if (!node.has("radius")) return defaultValue;
    Circle circle;
    circle.radius   = node.getFloat("radius", defaultValue.radius);
    circle.position = DataUtils::parsePosition(node, defaultValue.position);
    return circle;
}

Color DataUtils::parseColor(const DataNode& node, const Color& defaultColor)
{
    if (!node.has("r") || !node.has("g") || !node.has("b") || !node.has("a")) return defaultColor;
    return Color
    {
        static_cast<uint8_t>(node.getInt("r", defaultColor.r)),
        static_cast<uint8_t>(node.getInt("g", defaultColor.g)),
        static_cast<uint8_t>(node.getInt("b", defaultColor.b)),
        static_cast<uint8_t>(node.getInt("a", defaultColor.a))
    };
}

AABB DataUtils::parseAABB(const DataNode& node, const AABB& defaultValue)
{
    if (!node.has("left") || !node.has("right") || !node.has("top") || !node.has("bottom")) return defaultValue;

    AABB aabb;
    aabb.left   = node.getFloat("left", defaultValue.left);
    aabb.right  = node.getFloat("right", defaultValue.right);
    aabb.top    = node.getFloat("top", defaultValue.top);
    aabb.bottom = node.getFloat("bottom", defaultValue.bottom);
    return aabb;
}

Corners DataUtils::parseCorners(const DataNode& node, const Corners& defaultValue)
{
    if (!node.has("topLeft") || !node.has("topRight") || !node.has("bottomRight") || !node.has("bottomLeft")) return defaultValue;

    Corners corners;
    corners.topLeft     = node.getFloat("topLeft", defaultValue.topLeft);
    corners.topRight    = node.getFloat("topRight", defaultValue.topRight);
    corners.bottomRight = node.getFloat("bottomRight", defaultValue.bottomRight);
    corners.bottomLeft  = node.getFloat("bottomLeft", defaultValue.bottomLeft);
    return corners;
}

DebugConfig DataUtils::parseDebug(const DataNode& node, const DebugConfig& defaultConfig)
{
    if (!node.has("debug")) return defaultConfig;

    auto debugNode = node.getObject("debug");
    if (!debugNode) return defaultConfig;

    DebugConfig config;
    config.enabled = debugNode->getBool("enabled", defaultConfig.enabled);

    if (debugNode->has("color"))
    {
        auto colorNode = debugNode->getObject("color");
        if (colorNode) config.color = DataUtils::parseColor(*colorNode, defaultConfig.color);
        else config.color = defaultConfig.color;
    }
    else config.color = defaultConfig.color;
    
    config.layer   = debugNode->getInt("layer", defaultConfig.layer);
    config.zIndex  = debugNode->getInt("zIndex", defaultConfig.zIndex);
    config.filled  = debugNode->getBool("filled", defaultConfig.filled);
    return config;
}

void DataUtils::setPosition(DataNode& node, const Position& pos)
{
    node.setFloat("x", pos.x);
    node.setFloat("y", pos.y);
}

void DataUtils::setSize(DataNode& node, const Dimension2D& size)
{
    node.setFloat("width", size.width);
    node.setFloat("height", size.height);
}

void DataUtils::setRect(DataNode& node, const Rectangle& rect)
{
    if (node.has("position") && node.getObject("position"))
    {
        auto posNode = node.getObject("position");
        DataUtils::setPosition(*posNode, rect.position);
        node.setObject("position", std::move(posNode));
    }
    if (node.has("size") && node.getObject("size"))
    {
        auto sizeNode = node.getObject("size");
        DataUtils::setSize(*sizeNode, rect.size);
        node.setObject("size", std::move(sizeNode));
    }
}

void DataUtils::setCircle(DataNode& node, const Circle& circle)
{
    if (node.has("position") && node.getObject("position"))
    {
        auto posNode = node.getObject("position");
        DataUtils::setPosition(*posNode, circle.position);
        node.setObject("position", std::move(posNode));
    }
    if (node.has("radius")) node.setFloat("radius", circle.radius);
}

void DataUtils::setColor(DataNode& node, const Color& color)
{
    node.setInt("r", color.r);
    node.setInt("g", color.g);
    node.setInt("b", color.b);
    node.setInt("a", color.a);
}

void DataUtils::setAABB(DataNode& node, const AABB& aabb)
{
    node.setFloat("left", aabb.left);
    node.setFloat("right", aabb.right);
    node.setFloat("top", aabb.top);
    node.setFloat("bottom", aabb.bottom);
}

void DataUtils::setCorners(DataNode& node, const Corners& corners)
{
    node.setFloat("topLeft", corners.topLeft);
    node.setFloat("topRight", corners.topRight);
    node.setFloat("bottomRight", corners.bottomRight);
    node.setFloat("bottomLeft", corners.bottomLeft);
}
