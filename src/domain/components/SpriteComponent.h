#ifndef sprite_component_h
#define sprite_component_h

#include "Geometry/Geometry.h"
#include "Texture/Texture.h"

#include <memory>
#include <string>

struct SpriteComponent
{
    std::string texturePath;
    std::shared_ptr<Texture> cachedTexture;
    Dimension2D size {0.f, 0.f};
    Rectangle source {{ 0.f, 0.f }, { 0.f, 0.f }};
    bool useSourceRect = false;
};

#endif // sprite_component_h
