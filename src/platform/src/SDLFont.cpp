#include "../include/SDLFont/SDLFont.h"

#include <stdexcept>
#include <cmath>

SDLFont::SDLFont(const std::string& path) : path(path)
{
    this->baseFont = TTF_OpenFont(path.c_str(), 24);
    if (!this->baseFont) throw std::runtime_error("Failed to open font: " + path);

    this->sizeCache[24] = this->baseFont;
}

SDLFont::~SDLFont() { if (TTF_WasInit()) for (auto& [_, font] : this->sizeCache) TTF_CloseFont(font); }

TTF_Font* SDLFont::getFontWithSize(int size) const
{
    auto it = this->sizeCache.find(size);
    if (it != this->sizeCache.end()) return it->second;

    TTF_Font* newFont = TTF_OpenFont(this->path.c_str(), size);
    if (!newFont) throw std::runtime_error("Failed to create font size " + std::to_string(size));
    this->sizeCache[size] = newFont;
    return newFont;
}

int SDLFont::queryMetric(int size, int (TTF_Font::*metricFunc)() const) const
{
    TTF_Font* font = this->getFontWithSize(size);
    return (font->*metricFunc)();
}

int SDLFont::getAscent(int size) const { return TTF_FontAscent(this->getFontWithSize(size)); }

int SDLFont::getDescent(int size) const { return std::abs(TTF_FontDescent(this->getFontWithSize(size))); }

int SDLFont::getHeight(int size) const { return TTF_FontHeight(this->getFontWithSize(size)); }
