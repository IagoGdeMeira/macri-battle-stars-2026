#ifndef health_bar_system_h
#define health_bar_system_h

#include "DamageEvent.h"

#include "engine/include/System/System.h"

#include <vector>

class EventBus;
class World;

class HealthBarSystem : public System
{
public:
    HealthBarSystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    std::vector<DamageEvent> damageEvents;

    void processDamageEvent(World& world, const DamageEvent& event);
    void updateHealthBarSegments(World& world, Entity container, int currentHealth);
    void updateSegmentWidth(World& world, Entity segment, float newWidth);
};

#endif // health_bar_system_h
