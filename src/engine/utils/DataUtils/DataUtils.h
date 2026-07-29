#ifndef data_utils_h
#define data_utils_h

#include "DataNode/DataNode.h"
#include "DebugConfig/DebugConfig.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"


class DataUtils
{
public:
    static Position parsePosition(const DataNode& node, const Position& defaultValue = {0.f, 0.f});
    static Dimension2D parseSize(const DataNode& node, const Dimension2D& defaultValue = {0.f, 0.f});
    static Rectangle parseRect(const DataNode& node, const Rectangle& defaultValue = {{0.f, 0.f}, {0.f, 0.f}});
    static Circle parseCircle(const DataNode& node, const Circle& defaultValue = {{0.f, 0.f}, 0.f});
    static Color parseColor(const DataNode& node, const Color& defaultValue = {255, 255, 255, 255});
    static AABB parseAABB(const DataNode& node, const AABB& defaultValue = {0.f, 0.f, 0.f, 0.f});
    static Corners parseCorners(const DataNode& node, const Corners& defaultValue = {0.f, 0.f, 0.f, 0.f});
    
    static DebugConfig parseDebug(const DataNode& node, const DebugConfig& defaultConfig = {false, Color::WHITE()});

    static void setPosition(DataNode& node, const Position& pos);
    static void setSize(DataNode& node, const Dimension2D& size);
    static void setRect(DataNode& node, const Rectangle& rect);
    static void setCircle(DataNode& node, const Circle& circle);
    static void setColor(DataNode& node, const Color& color);
    static void setAABB(DataNode& node, const AABB& aabb);
    static void setCorners(DataNode& node, const Corners& corners);
};

#endif // data_utils_h
