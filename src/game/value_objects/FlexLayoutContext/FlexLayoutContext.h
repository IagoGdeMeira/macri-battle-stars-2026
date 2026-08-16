#ifndef flex_layout_context_h
#define flex_layout_context_h

#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <vector>

class World;

struct FlexContainer;

struct ChildLayoutInfo
{
    Entity entity{0};
    float mainSize = 0.f, crossSize = 0.f, grow = 0.f, shrink = 1.f;
    AABB margin{0, 0, 0, 0};
};

struct FlexLayoutContext
{
    World& world;
    Entity container;
    const FlexContainer& flex;
    std::vector<Entity> children;
    
    Rectangle innerRect;
    std::vector<ChildLayoutInfo> childInfos;

    bool isColumn = false, isReverse = false;
    float freeSpace = 0.f, startOffset = 0.f, between = 0.f;
};

#endif // flex_layout_context_h
