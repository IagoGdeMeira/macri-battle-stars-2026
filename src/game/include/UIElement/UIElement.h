#ifndef ui_element_h
#define ui_element_h

#include "../../../domain/components/BoxModel.h"
#include "../../../domain/include/Color/Color.h"
#include "../../../domain/include/Geometry/Geometry.h"

#include <string>

struct UIElement
{
    std::string id;
    Rectangle rect;
    BoxModel boxModel;

    virtual ~UIElement() = default;
};

struct PanelElement : public UIElement { Color bgColor = Color::TRANSPARENT(); };

struct TextElement : public UIElement
{
    std::string text;
    std::string fontPath = "assets/fonts/default.ttf";
    float fontSize = 24.0f;
    Color color = Color::WHITE();
};

struct ButtonElement : public UIElement { std::string actionId; };

struct ImageElement : public UIElement { std::string imagePath; };

#endif // ui_element_h
