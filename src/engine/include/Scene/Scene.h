#ifndef scene_h
#define scene_h

#include "../SystemManager/SystemManager.h"
#include "../UpdateContext/UpdateContext.h"

#include "../../domain/include/World/World.h"

class EventBus;

class Scene
{
public:
    Scene(EventBus& eventBus) : eventBus(eventBus) {}
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float deltaTime);

protected:
    World world;
    SystemManager systems;
    EventBus& eventBus;
};

#endif // scene_h
