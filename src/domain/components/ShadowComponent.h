#ifndef shadow_component_h
#define shadow_component_h

#include "../value_objects/Color/Color.h"
#include "../value_objects/Geometry/Geometry.h"

struct ShadowComponent
{
    Color color = Color::BLACK();
    Position offset { 2.f, 2.f };
    float blurRadius = 4.f;
    bool enabled = true;
};

#endif // shadow_component_h
