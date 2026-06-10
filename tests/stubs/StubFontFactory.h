#ifndef stub_font_factory_h
#define stub_font_factory_h

#include "../../src/engine/include/IFontFactory/IFontFactory.h"
#include "StubFont.h"

class StubFontFactory : public IFontFactory
{
public:
    int createFontCalls = 0;
    std::string lastPath;
    std::shared_ptr<Font> fontToReturn = std::make_shared<StubFont>();

    std::shared_ptr<Font> createFont(const std::string& path) override
    {
        ++this->createFontCalls;
        this->lastPath = path;
        return this->fontToReturn;
    }
};

#endif // stub_font_factory_h
