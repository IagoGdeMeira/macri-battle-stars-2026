#ifndef draw_rectangle_command_h
#define draw_rectangle_command_h

#include "BlendMode/BlendMode.h"
#include "DrawCommand/DrawCommand.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <cstddef>

struct DrawRectangleCommand : public DrawCommand
{
    Rectangle rect;
    Color color = Color::WHITE();
    bool filled = false;
    BlendMode blend = BlendMode::Normal;

    std::type_index type() const override { return typeid(DrawRectangleCommand); }
};

#endif // draw_rectangle_command_h
