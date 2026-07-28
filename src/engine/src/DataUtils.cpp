#include "DataUtils/DataUtils.h"

#include <stdexcept>

Position DataUtils::parsePosition(const DataNode& node, Position defaultValue)
{
    if (!node.has("x") || !node.has("y")) return defaultValue;
    return {node.getFloat("x", defaultValue.x), node.getFloat("y", defaultValue.y)};
}

Dimension2D DataUtils::parseSize(const DataNode& node, Dimension2D defaultValue)
{
    if (!node.has("w") || !node.has("h")) return defaultValue;
    return {node.getFloat("w", defaultValue.width), node.getFloat("h", defaultValue.height)};
}

Rectangle DataUtils::parseRect(const DataNode& node, Rectangle defaultValue)
{
    if (!node.has("position") || !node.has("size")) return defaultValue;
    Rectangle rect;
    rect.position = DataUtils::parsePosition(*node.getObject("position"), defaultValue.position);
    rect.size = DataUtils::parseSize(*node.getObject("size"), defaultValue.size);
    return rect;
}

Circle DataUtils::parseCircle(const DataNode& node, Circle defaultValue)
{
    if (!node.has("radius")) return defaultValue;
    Circle circle;
    circle.radius = node.getFloat("radius", defaultValue.radius);
    circle.position = DataUtils::parsePosition(node, defaultValue.position);
    return circle;
}

Color DataUtils::parseColor(const DataNode& node, Color defaultValue)
{
    if (!node.has("r") || !node.has("g") || !node.has("b") || !node.has("a")) return defaultValue;
    Color color;
    color.r = static_cast<uint8_t>(node.getInt("r", defaultValue.r));
    color.g = static_cast<uint8_t>(node.getInt("g", defaultValue.g));
    color.b = static_cast<uint8_t>(node.getInt("b", defaultValue.b));
    color.a = static_cast<uint8_t>(node.getInt("a", defaultValue.a));
    return color;
}

AABB DataUtils::parseAABB(const DataNode& node, AABB defaultValue)
{
    if (!node.has("left")) return defaultValue;
    if (!node.has("right")) return defaultValue;
    if (!node.has("top")) return defaultValue;
    if (!node.has("bottom")) return defaultValue;

    AABB aabb;
    aabb.left = node.getFloat("left", defaultValue.left);
    aabb.right = node.getFloat("right", defaultValue.right);
    aabb.top = node.getFloat("top", defaultValue.top);
    aabb.bottom = node.getFloat("bottom", defaultValue.bottom);
    return aabb;
}

Corners DataUtils::parseCorners(const DataNode& node, Corners defaultValue)
{
    if (!node.has("topLeft")) return defaultValue;
    if (!node.has("topRight")) return defaultValue;
    if (!node.has("bottomRight")) return defaultValue;
    if (!node.has("bottomLeft")) return defaultValue;

    Corners corners;
    corners.topLeft = node.getFloat("topLeft", defaultValue.topLeft);
    corners.topRight = node.getFloat("topRight", defaultValue.topRight);
    corners.bottomRight = node.getFloat("bottomRight", defaultValue.bottomRight);
    corners.bottomLeft = node.getFloat("bottomLeft", defaultValue.bottomLeft);
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
    }
    else config.color = defaultConfig.color;

    config.layer = debugNode->getInt("layer", defaultConfig.layer);
    config.zIndex = debugNode->getInt("zIndex", defaultConfig.zIndex);
    config.filled = debugNode->getBool("filled", defaultConfig.filled);
    
    return config;
}

void DataUtils::setPosition(DataNode& node, const Position& pos)
{
    node.setFloat("x", pos.x);
    node.setFloat("y", pos.y);
}

void DataUtils::setSize(DataNode& node, const Dimension2D& size)
{
    node.setFloat("w", size.width);
    node.setFloat("h", size.height);
}

void DataUtils::setRect(DataNode& node, const Rectangle& rect)
{
    if (node.has("position"))
    {
        auto posNode = node.getObject("position");
        if (!posNode) return;
    
        DataUtils::setPosition(*posNode, rect.position);
        node.setObject("position", std::move(posNode));
    }
    if (node.has("size"))
    {
        auto sizeNode = node.getObject("size");
        if (!sizeNode) return;
    
        DataUtils::setSize(*sizeNode, rect.size);
        node.setObject("size", std::move(sizeNode));
    }
}

void DataUtils::setCircle(DataNode& node, const Circle& circle)
{
    if (node.has("position"))
    {
        auto posNode = node.getObject("position");
        if (!posNode) return;

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
