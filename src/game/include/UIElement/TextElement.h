#ifndef text_element_h
#define text_element_h

#include "IUIAction/IUIAction.h"
#include "UIElement.h"

#include <memory>

struct TextElement : public UIElement
{
    std::string text;
    std::string fontPath = "assets/fonts/default.ttf";
    float fontSize = 24.f;
    Color color = Color::WHITE();
};

struct ButtonElement : public TextElement { std::shared_ptr<IUIAction> action; };

#endif // text_element_h
