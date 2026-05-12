#ifndef sdl_font_h
#define sdl_font_h

#include "../../../engine/include/Font/Font.h"

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

private:
    std::string path;
    TTF_Font* baseFont;
    mutable std::unordered_map<int, TTF_Font*> sizeCache;

    TTF_Font* getFontWithSize(int size) const;
    int queryMetric(int size, int (TTF_Font::*metricFunc)() const) const;
};

#endif // sdl_font_h
