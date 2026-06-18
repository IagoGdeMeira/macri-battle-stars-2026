#ifndef shape_render_component_h
#define shape_render_component_h

#include "../value_objects/CollisionFrame/ColliderDef.h"
#include "../value_objects/Color/Color.h"
#include "../value_objects/Geometry/Geometry.h"

#include <memory>

struct ShapeRenderComponent
{
    std::unique_ptr<ColliderDef> shape;
    Color color = { 255, 0, 0, 255 };
    bool filled = false;
    int layer = 0, zIndex = 0;
};

#endif // shape_render_component_h
