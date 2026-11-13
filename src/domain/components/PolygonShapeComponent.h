#ifndef polygon_shape_component_h
#define polygon_shape_component_h

#include "Color/Color.h"
#include "Geometry/Geometry.h"

struct PolygonShapeComponent
{
    Polygon polygon;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
};

#endif // polygon_shape_component_h
