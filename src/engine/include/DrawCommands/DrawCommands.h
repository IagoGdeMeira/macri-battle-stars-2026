#ifndef draw_commands_h
#define draw_commands_h

#include "../Font/Font.h"
#include "../Texture/Texture.h"

#include "../../domain/value_objects/Color/Color.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include <string>

enum class BlendMode : uint8_t { Normal, Add, Multiply };

class DrawCommand
{
public:
    enum class Type : uint8_t { Texture, Font, Rectangle, Circle };

    virtual ~DrawCommand() = default;
    virtual DrawCommand::Type type() const = 0;
};

struct DrawTextureCommand : public DrawCommand
{
  
    Texture* texture = nullptr;
    Rectangle dest;
    float rotation = 0.f;
    Position pivot = {0.5f, 0.5f};
    bool flipX = false, flipY = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;
    Rectangle source;
    bool useSourceRect = false;
    Color tint = Color::WHITE();
    BlendMode blend = BlendMode::Normal;

    DrawCommand::Type type() const override { return DrawCommand::Type::Texture; }
};

struct DrawFontCommand : public DrawCommand
{
    std::string text;
    Font* font = nullptr;
    Rectangle dest;
    int fontSize = 16;
    Color color = Color::WHITE();
    int layer = 0, zIndex = 0;
    size_t order = 0;

    DrawCommand::Type type() const override { return DrawCommand::Type::Font; }
};

struct DrawRectangleCommand : public DrawCommand
{
    Rectangle rect;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;

    DrawCommand::Type type() const override { return DrawCommand::Type::Rectangle; }
};

struct DrawCircleCommand : public DrawCommand
{
    Circle circle;
    Color color = Color::WHITE();
    bool filled = false;
    int layer = 0, zIndex = 0;
    size_t order = 0;

    DrawCommand::Type type() const override { return DrawCommand::Type::Circle; }
};

#endif // draw_commands_h
