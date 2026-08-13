#ifndef player_spawned_event_h
#define player_spawned_event_h

#include "domain/include/Entity/Entity.h"

#include <cstdint>
#include <string>

struct PlayerSpawnedEvent
{
    uint32_t playerId;
    std::string characterName;
    Entity playerEntity;
    int maxHealth, currentHealth;
};

#endif // player_spawned_event_h
