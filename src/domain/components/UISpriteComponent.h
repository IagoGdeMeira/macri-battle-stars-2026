#ifndef ui_sprite_component_h
#define ui_sprite_component_h

#include "../value_objects/Color/Color.h"

#include <memory>

class Texture;

struct UISpriteComponent
{
    std::shared_ptr<Texture> texture;
    Color tint = { 255, 255, 255, 255 };
};

#endif // ui_sprite_component_h
