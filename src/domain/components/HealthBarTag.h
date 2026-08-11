#ifndef health_bar_tag_h
#define health_bar_tag_h

#include <cstdint>

struct HealthBarTag { uint32_t playerId; int maxHealth = 100, currentHealth = 100; };

#endif // health_bar_tag_h
