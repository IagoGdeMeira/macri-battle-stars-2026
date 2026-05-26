#ifndef shadow_component_h
#define shadow_component_h

#include "../include/Color/Color.h"
#include "../include/Geometry/Geometry.h"

struct ShadowComponent
{
    Color color = Color::BLACK();
    Position offset { 2.0f, 2.0f };
    float blurRadius = 4.0f;
    bool enabled = true;
};

#endif // shadow_component_h
