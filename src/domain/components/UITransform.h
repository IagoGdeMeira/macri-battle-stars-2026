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

    Rectangle rect = { Position{0.f, 0.f}, Dimension2D{100.f, 100.f} };
    UIAnchor anchor = UIAnchor::TopLeft;
    Position pivot {0.5f, 0.5f}, scale {1.f, 1.f};
    float rotation = 0.0f;
};

#endif // ui_transform_h
