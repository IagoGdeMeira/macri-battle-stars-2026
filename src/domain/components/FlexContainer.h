#ifndef flex_container_h
#define flex_container_h

#include "FlexEnums/FlexEnums.h"

struct FlexContainer
{
    FlexDirection direction = FlexDirection::Row;
    JustifyContent justify = JustifyContent::FlexStart;
    AlignItems align = AlignItems::Stretch;

    float gap = 0.f;
};

#endif // flex_container_h
