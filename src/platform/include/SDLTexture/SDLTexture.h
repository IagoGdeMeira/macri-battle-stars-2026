#ifndef sdl_texture_h
#define sdl_texture_h

#include "../../engine/include/Texture/Texture.h"

#include <SDL.h>

class SDLTexture : public Texture
{
public:
    SDLTexture(SDL_Texture* texture) : texture(texture) {}
    ~SDLTexture();

    SDL_Texture* get() const { return this->texture; }

    int getWidth() const override;
    int getHeight() const override;

private:
    SDL_Texture* texture = nullptr;
};

#endif // sdl_texture_h
