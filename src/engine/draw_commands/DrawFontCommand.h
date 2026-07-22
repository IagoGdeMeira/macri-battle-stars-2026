#ifndef draw_font_command_h
#define draw_font_command_h

#include "DrawCommand/DrawCommand.h"
#include "Font/Font.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <string>

struct DrawFontCommand : public DrawCommand
{
    Font* font = nullptr;
    std::string text;
    Rectangle dest;
    float fontSize = 12.f;
    Color color = Color::WHITE();
    int layer = 0, zIndex = 0;
    size_t order = 0;

    std::type_index type() const override { return typeid(DrawFontCommand); }
};

#endif // draw_font_command_h
