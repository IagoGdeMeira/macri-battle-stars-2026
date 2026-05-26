#ifndef outline_component_h
#define outline_component_h

#include "../include/Color/Color.h"

struct OutlineComponent
{
    Color color = Color::BLACK();
    float thickness = 2.0f, blurRadius = 0.0f;
    bool enabled = true;
};

#endif // outline_component_h
