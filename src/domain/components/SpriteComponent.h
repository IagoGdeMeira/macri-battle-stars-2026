#ifndef sprite_component_h
#define sprite_component_h

#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/Texture/Texture.h"

#include <memory>

struct SpriteComponent
{
    std::shared_ptr<Texture> texture;

    Dimension2D size {0.f, 0.f};
    Rectangle source {{ 0.f, 0.f }, { 0.f, 0.f }};
    bool useSourceRect = false;
};

#endif // sprite_component_h
