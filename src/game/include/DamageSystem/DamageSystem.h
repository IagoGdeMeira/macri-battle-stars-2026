#ifndef damage_system_h
#define damage_system_h

#include "../../engine/include/System/System.h"

#include <vector>

class EventBus;

struct CollisionEvent;

class DamageSystem : public System
{
public:
    DamageSystem(EventBus& bus);

    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    std::vector<CollisionEvent> collisions;
};

#endif // damage_system_h
