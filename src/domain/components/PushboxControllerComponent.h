#ifndef pushbox_controller_component_h
#define pushbox_controller_component_h

#include "Entity/Entity.h"

#include <vector>

struct PushboxControllerComponent
{
    struct Frame { std::vector<Entity> pushboxes; float duration = 0.f; };

    std::vector<Frame> frames;
    int currentFrame = 0;
    float frameDuration = 0.1f, elapsedTime = 0.f;
    bool loop = false, initialized = false, finished = false;
};

#endif // pushbox_controller_component_h
