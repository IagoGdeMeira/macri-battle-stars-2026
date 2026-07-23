#ifndef draw_rectangle_command_h
#define draw_rectangle_command_h

#include "DrawCommand/DrawCommand.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <cstddef>

struct DrawRectangleCommand : public DrawCommand
{
    Rectangle rect;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;

    std::type_index type() const override { return typeid(DrawRectangleCommand); }
};

#endif // draw_rectangle_command_h
