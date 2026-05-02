#ifndef sdl_texture_h
#define sdl_texture_h

#include "../../../engine/include/Texture/Texture.h"

#include <SDL.h>

class SDLTexture : public Texture
{
public:
    SDLTexture(SDL_Texture* texture) : texture(texture) {}
    ~SDLTexture() { if (this->texture) SDL_DestroyTexture(this->texture); }

    SDL_Texture* get() const { return this->texture; }

    int getWidth() const override
    {
        int width = 0;
        if (this->texture) SDL_QueryTexture(this->texture, nullptr, nullptr, &width, nullptr);
        return width;
    }

    int getHeight() const override
    {
        int height = 0;
        if (this->texture) SDL_QueryTexture(this->texture, nullptr, nullptr, nullptr, &height);
        return height;
    }

private:
    SDL_Texture* texture = nullptr;
};

#endif // sdl_texture_h
