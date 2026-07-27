#ifndef hitbox_controller_component_h
#define hitbox_controller_component_h

#include "Entity/Entity.h"

#include <vector>

struct HitboxControllerComponent
{
    struct Frame { std::vector<Entity> hitboxes; float duration = 0.f; };

    std::vector<Frame> frames;
    int currentFrame = 0;
    float elapsedTime = 0.f;
    bool loop = false, initialized = false;
};

#endif // hitbox_controller_component_h
