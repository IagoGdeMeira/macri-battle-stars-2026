#ifndef ui_text_component_h
#define ui_text_component_h

#include "Color/Color.h"

#include <memory>
#include <string>

class Font;

struct UITextComponent
{
    std::shared_ptr<Font> font;
    std::string text;
    Color color = { 255, 255, 255, 255 };
    bool needsUpdate = true;
    float fontSize = -1.f, letterSpacing = 0.f, lineSpacing = 0.f;
};

#endif // ui_text_component_h
