#ifndef sdl_font_factory_h
#define sdl_font_factory_h

#include "../include/IFontFactory/IFontFactory.h"

class SDLFontFactory : public IFontFactory
{
public:
    std::shared_ptr<Font> createFont(const std::string& path) override;
};

#endif // sdl_font_factory_h
