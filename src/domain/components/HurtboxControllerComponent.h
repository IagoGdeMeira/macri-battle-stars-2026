#ifndef hurtbox_controller_component_h
#define hurtbox_controller_component_h

#include "Entity/Entity.h"

#include <vector>

struct HurtboxControllerComponent
{
    struct Frame { std::vector<Entity> hurtboxes; float duration = 0.f; };

    std::vector<Frame> frames;
    int currentFrame = 0;
    float frameDuration = 0.1f, elapsedTime = 0.f;
    bool loop = false, initialized = false, finished = false;
};

#endif // hurtbox_controller_component_h
