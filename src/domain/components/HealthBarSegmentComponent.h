#ifndef health_bar_segment_component_h
#define health_bar_segment_component_h

#include "Color/Color.h"

struct HealthBarSegmentComponent
{
    float maxHP = 100.f, maxWidth = 0.f;
    Color fill = Color{0, 220, 0, 255}, shadow = {0, 90, 0, 255};
    int segmentIndex = 0;
};

#endif // health_bar_segment_component_h
