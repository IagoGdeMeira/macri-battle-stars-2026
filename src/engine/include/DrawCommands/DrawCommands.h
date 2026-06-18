#ifndef draw_commands_h
#define draw_commands_h

#include "../Font/Font.h"
#include "../Texture/Texture.h"

#include "../../domain/value_objects/Color/Color.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include <string>

enum class BlendMode { Normal, Add, Multiply };

struct DrawTextureCommand
{
    Texture* texture = nullptr;
    Rectangle dest;
    float rotation = 0.0f;
    Position pivot = {0.5f, 0.5f};
    bool flipX = false, flipY = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;
    Rectangle source;
    bool useSourceRect = false;
    Color tint = Color::WHITE();
    BlendMode blend = BlendMode::Normal;
};

struct DrawFontCommand
{
    std::string text;
    Font* font = nullptr;
    Rectangle dest;
    int fontSize = 16;
    Color color = Color::WHITE();
    int layer = 0, zIndex = 0;
    size_t order = 0;
};

struct DrawRectangleCommand
{
    Rectangle rect;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;
};

struct DrawCircleCommand
{
    Circle circle;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;
};

#endif // draw_commands_h
