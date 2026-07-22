#ifndef draw_texture_command_h
#define draw_texture_command_h

#include "DrawCommand/DrawCommand.h"
#include "Texture/Texture.h"

#include "domain/value_objects/Color/Color.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include <memory>

enum class BlendMode { Normal, Add, Multiply };

struct DrawTextureCommand : public DrawCommand
{
    std::shared_ptr<Texture> texture = nullptr;
    Rectangle dest, source;
    bool useSourceRect = false;
    float rotation = 0.f;
    Position pivot = {0.5f, 0.5f};
    bool flipX = false, flipY = false;
    Color tint = Color::WHITE();
    BlendMode blend = BlendMode::Normal;
    int layer = 0, zIndex = 0;
    size_t order = 0;

    std::type_index type() const override { return typeid(DrawTextureCommand); }
};

#endif // draw_texture_command_h
