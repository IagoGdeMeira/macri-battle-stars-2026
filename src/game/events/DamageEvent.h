#ifndef damage_event_h
#define damage_event_h

#include "Entity/Entity.h"

#include <cstdint>

struct DamageEvent
{
    Entity attacker, target;
    uint32_t targetPlayerId;
    int damage = 0, remainingHealth = 0;
};

#endif // damage_event_h
