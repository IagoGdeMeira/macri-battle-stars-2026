#ifndef ui_text_component_h
#define ui_text_component_h

#include "../include/Color/Color.h"

#include <memory>
#include <string>

class Font;

struct UITextComponent
{
    std::shared_ptr<Font> font;
    std::string text;
    Color color = { 255, 255, 255, 255 };
    bool needsUpdate = true;
    float fontSize = -1.0f, letterSpacing = 0.0f, lineSpacing = 0.0f;
};

#endif // ui_text_component_h
