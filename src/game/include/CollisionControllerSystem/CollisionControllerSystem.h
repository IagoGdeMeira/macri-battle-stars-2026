#ifndef collision_controller_system_h
#define collision_controller_system_h

#include "StateChangedEvent.h"

#include "engine/include/System/System.h"

#include <vector>

class EventBus;

class CollisionControllerSystem : public System
{
public:
    explicit CollisionControllerSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    std::vector<StateChangedEvent> pendingEvents;
};

#endif // collision_controller_system_h
