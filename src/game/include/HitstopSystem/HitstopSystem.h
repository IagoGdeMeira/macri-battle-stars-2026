#ifndef hitstop_system_h
#define hitstop_system_h

#include "engine/include/EventBus/EventBus.h"
#include "engine/include/System/System.h"

#include <vector>

struct DamageEvent;

class HitstopSystem : public System
{
public:
    explicit HitstopSystem(EventBus& bus, float duration = 0.1f);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    float hitstopDuration;
    std::vector<DamageEvent> damageEvents;
};

#endif // hitstop_system_h
