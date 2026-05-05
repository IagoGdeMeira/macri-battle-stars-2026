#ifndef knockback_system_h
#define knockback_system_h

#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/System/System.h"

#include <vector>

struct DamageEvent;

class KnockbackSystem : public System
{
public:
    explicit KnockbackSystem(EventBus& bus);

    void update(UpdateContext& ctx) override;

private:
    EventBus &bus;
    std::vector<DamageEvent> damageEvents;
};

#endif // knockback_system_h
