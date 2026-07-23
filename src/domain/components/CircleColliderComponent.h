#ifndef circle_collider_component_h
#define circle_collider_component_h

#include "domain/value_objects/Geometry/Geometry.h"

struct CircleColliderComponent
{
    float radius = 0.f;
    AABB cachedAABB{0.f, 0.f, 0.f, 0.f};
};

#endif // circle_collider_component_h
