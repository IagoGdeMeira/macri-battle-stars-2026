#ifndef stub_font_factory_h
#define stub_font_factory_h

#include "StubFont.h"

#include "engine/include/IFontFactory/IFontFactory.h"

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
