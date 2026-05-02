#ifndef shape_render_component_h
#define shape_render_component_h

#include "../include/CollisionFrame/ColliderDef.h"
#include "../include/Color/Color.h"

#include "../../engine/include/GeometryUtils/GeometryUtils.h"
#include "../../engine/include/Renderer/Renderer.h"

#include <memory>

struct ShapeRenderComponent
{
    std::unique_ptr<ColliderDef> shape;
    Color color = { 255, 0, 0, 255 };
    bool filled = false;
    int layer = 0, zIndex = 0;
};

#endif // shape_render_component_h
