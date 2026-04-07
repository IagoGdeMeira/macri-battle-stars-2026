#ifndef sprite_component_h
#define sprite_component_h

#include <string>

struct SpriteComponent
{
    std::string textureId;

    int width = 0;
    int height = 0;
};

#endif // sprite_component_h
