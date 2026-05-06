#ifndef sdl_texture_factory_h
#define sdl_texture_factory_h

#include "../../engine/include/ITextureFactory/ITextureFactory.h"

#include <SDL.h>

class SDLTextureFactory : public ITextureFactory
{
public:
    explicit SDLTextureFactory(SDL_Renderer* renderer) : renderer(renderer) {}

    std::shared_ptr<Texture> createTexture(const std::string& path) override;

private:
    SDL_Renderer* renderer;
};

#endif // sdl_texture_factory_h
