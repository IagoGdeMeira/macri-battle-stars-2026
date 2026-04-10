#ifndef sprite_component_h
#define sprite_component_h

#include "../../engine/include/Texture/Texture.h"

#include <memory>

struct SpriteComponent
{
    std::shared_ptr<Texture> texture;

    int width = 0;
    int height = 0;
};

#endif // sprite_component_h
