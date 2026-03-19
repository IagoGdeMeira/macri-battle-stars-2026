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

    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

protected:
    World world;
    SystemManager systems;
    EventBus& eventBus;
};

#include "Scene.inl"

#endif // scene_h
