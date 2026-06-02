#ifndef data_utils_h
#define data_utils_h

#include "../DataNode/DataNode.h"

#include "../../domain/include/Geometry/Geometry.h"
#include "../../domain/include/Color/Color.h"

class DataUtils
{
public:
    static Position parsePosition(const DataNode& node, Position defaultValue = {0.f, 0.f});
    static Dimension2D parseSize(const DataNode& node, Dimension2D defaultValue = {0.f, 0.f});
    static Rectangle parseRect(const DataNode& node, Rectangle defaultValue = {{0.f, 0.f}, {0.f, 0.f}});
    static Color parseColor(const DataNode& node, Color defaultValue = {255, 255, 255, 255});
    static AABB parseAABB(const DataNode& node, AABB defaultValue = {0.f, 0.f, 0.f, 0.f});
    static Corners parseCorners(const DataNode& node, Corners defaultValue = {0.f, 0.f, 0.f, 0.f});
};

#endif // data_utils_h
