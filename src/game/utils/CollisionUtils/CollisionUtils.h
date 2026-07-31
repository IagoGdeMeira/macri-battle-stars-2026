#ifndef collision_utils_h
#define collision_utils_h

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

class CollisionUtils
{
public:
    static void updateWorldTransform(World& world, Entity child, Entity parent);
};

#endif // collision_utils_h
