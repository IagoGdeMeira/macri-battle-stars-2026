#ifndef damage_event_h
#define damage_event_h

#include "Entity/Entity.h"

struct DamageEvent { Entity attacker, target; int damage = 0; };

#endif // damage_event_h
