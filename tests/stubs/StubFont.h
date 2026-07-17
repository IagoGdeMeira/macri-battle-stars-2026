#ifndef stub_font_h
#define stub_font_h

#include "../../src/engine/resources/Font/Font.h"

class StubFont : public Font
{
public:
    int getAscent(int fontSize) const override { (void)fontSize; return 8; }
    int getDescent(int fontSize) const override { (void)fontSize; return 2; }
    int getHeight(int fontSize) const override { (void)fontSize; return 10; }
};

#endif // stub_font_h
