#ifndef ui_transform_h
#define ui_transform_h

#include "../include/Geometry/Geometry.h"

struct UITransform
{
    enum class UIAnchor
    {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight
    };

    Rectangle rect = { {0.0f, 0.0f}, 100.0f, 100.0f };
    UIAnchor anchor = UIAnchor::TopLeft;
    Position pivot { 0.5f, 0.5f }, scale { 1.0f, 1.0f };
    float rotation = 0.0f;
};

#endif // ui_transform_h
