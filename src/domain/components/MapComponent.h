#ifndef map_component_h
#define map_component_h

#include "../value_objects/Geometry/Geometry.h"

#include <string>
#include <vector>

struct MapComponent
{
    struct SpawnPoint { std::uint32_t playerId; float x; };

    std::string name;
    float gravity = 980.f, floorFriction = 5.f, airFriction = 2.f, floorY = 0.f;
    AABB worldBounds;
    std::vector<SpawnPoint> spawnPoints;
};

#endif // map_component_h
