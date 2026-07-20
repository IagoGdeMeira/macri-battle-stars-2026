#ifndef data_utils_h
#define data_utils_h

#include "DataNode/DataNode.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

class DataUtils
{
public:
    static Position parsePosition(const DataNode& node, Position defaultValue = {0.f, 0.f});
    static Dimension2D parseSize(const DataNode& node, Dimension2D defaultValue = {0.f, 0.f});
    static Rectangle parseRect(const DataNode& node, Rectangle defaultValue = {{0.f, 0.f}, {0.f, 0.f}});
    static Color parseColor(const DataNode& node, Color defaultValue = {255, 255, 255, 255});
    static AABB parseAABB(const DataNode& node, AABB defaultValue = {0.f, 0.f, 0.f, 0.f});
    static Corners parseCorners(const DataNode& node, Corners defaultValue = {0.f, 0.f, 0.f, 0.f});

    static void setPosition(DataNode& node, const Position& pos);
    static void setSize(DataNode& node, const Dimension2D& size);
    static void setRect(DataNode& node, const Rectangle& rect);
    static void setColor(DataNode& node, const Color& color);
    static void setAABB(DataNode& node, const AABB& aabb);
    static void setCorners(DataNode& node, const Corners& corners);
};

#endif // data_utils_h
