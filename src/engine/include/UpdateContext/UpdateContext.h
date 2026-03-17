#ifndef update_context_h
#define update_context_h

#include "../../domain/include/World/World.h"

class EventBus;

struct UpdateContext
{
    World& world;
    EventBus& eventBus;
    float deltaTime;
};

#endif // update_context_h
