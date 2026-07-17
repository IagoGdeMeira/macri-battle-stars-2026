#ifndef i_collision_detection_h
#define i_collision_detection_h

#include "../../domain/include/Entity/Entity.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

#include <vector>

class ICollisionDetection
{
public:
    struct CollisionPair { Entity a, b; };

    virtual ~ICollisionDetection() = default;

    virtual void detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx) = 0;
};

#endif // i_collision_detection_h
