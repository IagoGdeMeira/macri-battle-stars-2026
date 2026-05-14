#ifndef box_model_h
#define box_model_h

#include "../include/Geometry/Geometry.h"

struct BoxModel
{
    Rectangle margin;
    Rectangle padding;
    Rectangle border;
    Corners borderRadius;
};

#endif // box_model_h
