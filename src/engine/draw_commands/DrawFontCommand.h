#ifndef draw_font_command_h
#define draw_font_command_h

#include "DrawCommand/DrawCommand.h"

#include "domain/resources/Font/Font.h"
#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <cstddef>
#include <string>

struct DrawFontCommand : public DrawCommand
{
    Font* font = nullptr;
    std::string text;
    Rectangle dest;
    float fontSize = 12.f;
    Color color = Color::WHITE();

    std::type_index type() const override { return typeid(DrawFontCommand); }
};

#endif // draw_font_command_h
