#ifndef font_loader_h
#define font_loader_h

#include "IFontFactory/IFontFactory.h"
#include "ResourceLoader/ResourceLoader.h"

#include "domain/resources/Font/Font.h"

class FontLoader : public ResourceLoader<Font>
{
public:
    explicit FontLoader(IFontFactory& factory) : factory(factory) {}
    
    std::shared_ptr<Font> load(const std::string& path) override
    { return this->factory.createFont(path); }

private:
    IFontFactory& factory;
};

#endif // font_loader_h
