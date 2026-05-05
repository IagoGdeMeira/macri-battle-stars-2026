#ifndef damage_event_h
#define damage_event_h

#include "../include/Entity/Entity.h"

struct DamageEvent
{
    Entity& attacker;
    Entity& target;
    int damage;
};

#endif // damage_event_h
