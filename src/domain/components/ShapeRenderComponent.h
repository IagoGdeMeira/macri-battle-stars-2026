#ifndef shape_render_component_h
#define shape_render_component_h

#include "CollisionFrame/ColliderDef.h"
#include "Color/Color.h"
#include "Geometry/Geometry.h"

#include <memory>

struct ShapeRenderComponent
{
    std::unique_ptr<ColliderDef> shape;
    Color color = {255, 0, 0, 255};
    bool filled = false;
    int layer = 0, zIndex = 0;
};

#endif // shape_render_component_h
