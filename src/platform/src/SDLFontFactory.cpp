#include "SDLFontFactory/SDLFontFactory.h"

#include "SDLFont/SDLFont.h"

std::shared_ptr<Font> SDLFontFactory::createFont(const std::string& path)
{ return std::make_shared<SDLFont>(path); }
