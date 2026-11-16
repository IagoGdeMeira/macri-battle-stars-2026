#ifndef health_bar_system_h
#define health_bar_system_h

#include "DamageEvent.h"

#include "domain/value_objects/Color/Color.h"

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
    
    struct EndCapData { int currentHealth; float barWidth; Color color; };
    void updateEndCap(World& world, Entity container, Entity endCap, EndCapData data);
};

#endif // health_bar_system_h
