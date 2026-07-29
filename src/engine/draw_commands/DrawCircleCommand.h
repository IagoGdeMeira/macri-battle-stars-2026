#ifndef draw_circle_command_h
#define draw_circle_command_h

#include "BlendMode/BlendMode.h"
#include "DrawCommand/DrawCommand.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <cstddef>

struct DrawCircleCommand : public DrawCommand
{
    Circle circle;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;
    BlendMode blend = BlendMode::Normal;

    std::type_index type() const override { return typeid(DrawCircleCommand); }
};

#endif // draw_circle_command_h
