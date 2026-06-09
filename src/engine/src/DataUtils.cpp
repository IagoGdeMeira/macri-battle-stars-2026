#include "../include/DataUtils/DataUtils.h"

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
    
    return
    {
        DataUtils::parsePosition(*node.getObject("position"), defaultValue.position),
        DataUtils::parseSize(*node.getObject("size"), defaultValue.size)
    };
}

Color DataUtils::parseColor(const DataNode& node, Color defaultValue)
{
    if (!node.has("r")) return defaultValue;
    if (!node.has("g")) return defaultValue;
    if (!node.has("b")) return defaultValue;
    if (!node.has("a")) return defaultValue;
    
    return
    {
        static_cast<uint8_t>(node.getInt("r", defaultValue.r)),
        static_cast<uint8_t>(node.getInt("g", defaultValue.g)),
        static_cast<uint8_t>(node.getInt("b", defaultValue.b)),
        static_cast<uint8_t>(node.getInt("a", defaultValue.a))
    };
}

AABB DataUtils::parseAABB(const DataNode& node, AABB defaultValue)
{
    if (!node.has("left")) return defaultValue;
    if (!node.has("right")) return defaultValue;
    if (!node.has("top")) return defaultValue;
    if (!node.has("bottom")) return defaultValue;

    return
    {
        node.getFloat("left", defaultValue.left),
        node.getFloat("right", defaultValue.right),
        node.getFloat("top", defaultValue.top),
        node.getFloat("bottom", defaultValue.bottom)
    };
}

Corners DataUtils::parseCorners(const DataNode& node, Corners defaultValue)
{
    if (!node.has("topLeft")) return defaultValue;
    if (!node.has("topRight")) return defaultValue;
    if (!node.has("bottomRight")) return defaultValue;
    if (!node.has("bottomLeft")) return defaultValue;

    return
    {
        node.getFloat("topLeft", defaultValue.topLeft),
        node.getFloat("topRight", defaultValue.topRight),
        node.getFloat("bottomRight", defaultValue.bottomRight),
        node.getFloat("bottomLeft", defaultValue.bottomLeft)
    };
}

void DataUtils::setPosition(DataNode& node, const Position& pos)
{
    node.setFloat("x", pos.x);
    node.setFloat("y", pos.y);
}
