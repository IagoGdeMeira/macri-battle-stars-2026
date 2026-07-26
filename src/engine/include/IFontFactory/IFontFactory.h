#ifndef i_font_factory_h
#define i_font_factory_h

#include "domain/resources/Font/Font.h"

#include <memory>
#include <string>

class IFontFactory
{
public:
    virtual ~IFontFactory() = default;
    virtual std::shared_ptr<Font> createFont(const std::string& path) = 0;
};

#endif // i_font_factory_h
