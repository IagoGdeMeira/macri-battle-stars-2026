#ifndef scene_h
#define scene_h

#include "../SystemManager/SystemManager.h"
#include "../UpdateContext/UpdateContext.h"

#include "../../domain/include/World/World.h"

class EventBus;

class Scene
{
public:
    enum class UpdatePolicy { Always, WhenTop, Never };

    Scene(EventBus& eventBus) : eventBus(eventBus) {}
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float deltaTime);
    virtual void render() {}

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onPause() {}
    virtual void onResume() {}

    virtual UpdatePolicy getUpdatePolicy() const { return UpdatePolicy::WhenTop; }

    World& world() { return this->localWorld; }
    const World& world() const { return this->localWorld; }

    SystemManager& systems() { return this->systemManager; }
    const SystemManager& systems() const { return this->systemManager; }

    template <typename T, typename... Args>
    T& addSystem(Args&&... args);

protected:
    World localWorld; 
    SystemManager systemManager;
    EventBus& eventBus;
};

#include "Scene.inl"

#endif // scene_h
