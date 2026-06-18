#ifndef local_transform_h
#define local_transform_h

#include "../value_objects/Geometry/Geometry.h"

struct LocalTransform
{
    Position position{0.f, 0.f}, scale{1.f, 1.f};
    float rotation = 0.0f;
};

#endif // local_transform_h
