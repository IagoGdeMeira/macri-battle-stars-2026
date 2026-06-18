#ifndef transform_component_h
#define transform_component_h

#include "../value_objects/Geometry/Geometry.h"

struct TransformComponent
{
    Position position {0.f, 0.f}, scale {1.f, 1.f};
    float rotation = 0.f;
};

#endif // transform_component_h
