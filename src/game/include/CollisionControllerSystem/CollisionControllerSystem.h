#ifndef collision_controller_system_h
#define collision_controller_system_h

#include "ICollisionController/ICollisionController.h"
#include "StateChangedEvent.h"

#include "domain/events/OrientationChangedEvent.h"

#include "engine/include/System/System.h"

#include <vector>
#include <memory>

class EventBus;
class World;

class CollisionControllerSystem : public System
{
public:
    explicit CollisionControllerSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;
    
    void addController(std::unique_ptr<ICollisionController> controller);

private:
    EventBus& bus;
    std::vector<std::unique_ptr<ICollisionController>> controllers;

    std::vector<StateChangedEvent> stateChangedEvents;
    std::vector<OrientationChangedEvent> orientationChangedEvents;
};

#endif // collision_controller_system_h
