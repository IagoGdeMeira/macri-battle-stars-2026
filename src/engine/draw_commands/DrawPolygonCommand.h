#ifndef draw_polygon_command_h
#define draw_polygon_command_h

#include "BlendMode/BlendMode.h"
#include "DrawCommand/DrawCommand.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

struct DrawPolygonCommand : public DrawCommand
{
    Polygon polygon;
    Color color = Color::WHITE();
    bool filled = false;
    BlendMode blend = BlendMode::Normal;

    std::type_index type() const override { return typeid(DrawPolygonCommand); }
};

#endif // draw_polygon_command_h
