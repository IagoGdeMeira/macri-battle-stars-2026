#ifndef ui_layout_metrics_component_h
#define ui_layout_metrics_component_h

#include "domain/value_objects/Geometry/Geometry.h"

struct UILayoutMetricsComponent
{
    enum class UIAnchor
    {
        TopLeft, TopCenter, TopRight,
        CenterLeft, Center, CenterRight,
        BottomLeft, BottomCenter, BottomRight
    };

    Dimension2D size{100.f, 100.f};
    UIAnchor anchor = UIAnchor::TopLeft;
    Position pivot{0.5f, 0.5f};
};

#endif // ui_layout_metrics_component_h
