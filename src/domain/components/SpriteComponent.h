#ifndef sprite_component_h
#define sprite_component_h

#include "../../engine/include/Texture/Texture.h"

#include <memory>

struct SpriteComponent
{
    std::shared_ptr<Texture> texture;

    int width = 0, height = 0;
    
    int srcX = 0, srcY = 0, srcWidth = 0, srcHeight = 0;
    bool useSourceRect = false;
};

#endif // sprite_component_h
