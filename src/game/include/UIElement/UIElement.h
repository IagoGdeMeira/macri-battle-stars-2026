#ifndef ui_element_h
#define ui_element_h

#include "../../domain/components/BoxModel.h"
#include "../../domain/include/Color/Color.h"
#include "../../domain/include/Geometry/Geometry.h"

#include <string>

struct UIElement
{
    std::string id;
    Rectangle rect;
    BoxModel boxModel;
    int layer = 0, zIndex = 0;

    virtual ~UIElement() = default;
};

struct PanelElement : public UIElement { Color bgColor = Color::TRANSPARENT(); };

struct ImageElement : public UIElement { std::string imagePath; };

#endif // ui_element_h
