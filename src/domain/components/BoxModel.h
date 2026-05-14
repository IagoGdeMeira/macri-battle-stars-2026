#ifndef box_model_h
#define box_model_h

#include "../include/Geometry/Geometry.h"

struct BoxModel
{
    AABB margin, padding, border;
    Corners borderRadius;
};

#endif // box_model_h
