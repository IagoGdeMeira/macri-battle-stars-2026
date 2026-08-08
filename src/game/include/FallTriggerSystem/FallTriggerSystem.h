#ifndef fall_trigger_system_h
#define fall_trigger_system_h

#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"
#include "engine/include/EventBus/EventBus.h"

#include <unordered_map>

class FallTriggerSystem : public System
{
public:
    explicit FallTriggerSystem(EventBus& bus) : bus(bus) {}
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    std::unordered_map<Entity, float, Entity::Hash> previousVelocityY;
};

#endif // fall_trigger_system_h
