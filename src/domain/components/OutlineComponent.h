#ifndef outline_component_h
#define outline_component_h

#include "Color/Color.h"

struct OutlineComponent
{
    Color color = Color::BLACK();
    float thickness = 2.f, blurRadius = 0.f;
    bool enabled = true;
};

#endif // outline_component_h
