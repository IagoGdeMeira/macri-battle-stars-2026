#include "../include/SDLTexture/SDLTexture.h"

SDLTexture::~SDLTexture() { if (this->texture) SDL_DestroyTexture(this->texture); }

int SDLTexture::getWidth() const
{
    if (!this->texture) return 0;

    int w = 0;
    if (SDL_QueryTexture(this->texture, nullptr, nullptr, &w, nullptr) != 0) return 0;
    return w;
}

int SDLTexture::getHeight() const
{
    if (!this->texture) return 0;

    int h = 0;
    if (SDL_QueryTexture(this->texture, nullptr, nullptr, nullptr, &h) != 0) return 0;
    return h;
}
