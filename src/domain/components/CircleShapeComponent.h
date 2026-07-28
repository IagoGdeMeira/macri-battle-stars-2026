#ifndef circle_shape_component_h
#define circle_shape_component_h

#include "Color/Color.h"
#include "Geometry/Geometry.h"

struct CircleShapeComponent
{
    Circle circle;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
};

#endif // circle_shape_component_h
