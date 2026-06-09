#ifndef stub_font_factory_h
#define stub_font_factory_h

#include "../../src/engine/include/IFontFactory/IFontFactory.h"

class StubFontFactory : public IFontFactory
{
public:
    std::shared_ptr<Font> createFont(const std::string&) override { return nullptr; }
};

#endif // stub_font_factory_h
