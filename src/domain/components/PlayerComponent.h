#ifndef player_component_h
#define player_component_h

#include <cstdint>

using PlayerId = uint32_t;

struct PlayerComponent
{
    PlayerId id;
};

#endif // player_component_h