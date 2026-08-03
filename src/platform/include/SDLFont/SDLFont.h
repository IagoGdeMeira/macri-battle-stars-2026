#ifndef sdl_font_h
#define sdl_font_h

#include "domain/resources/Font/Font.h"

#include <SDL_ttf.h>
#include <string>
#include <unordered_map>

class SDLFont : public Font
{
public:
    explicit SDLFont(const std::string& path);
    ~SDLFont() override;

    int getAscent(int size) const override;
    int getDescent(int size) const override;
    int getHeight(int size) const override;

    TTF_Font* getFontWithSize(int size) const;

private:
    std::string path;
    TTF_Font* baseFont;
    mutable std::unordered_map<int, TTF_Font*> sizeCache;
};

#endif // sdl_font_h
