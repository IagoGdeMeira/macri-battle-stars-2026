#ifndef rectangle_shape_component_h
#define rectangle_shape_component_h

#include "Color/Color.h"
#include "Geometry/Geometry.h"

struct RectangleShapeComponent
{
    Rectangle rect;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0;
};

#endif // rectangle_shape_component_h
