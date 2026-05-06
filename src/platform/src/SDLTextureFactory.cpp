#include "../include/SDLTextureFactory/SDLTextureFactory.h"

#include "../include/SDLTexture/SDLTexture.h"

#include <iostream>
#include <SDL_image.h>

std::shared_ptr<Texture> SDLTextureFactory::createTexture(const std::string& path)
{
    SDL_Texture* tex = IMG_LoadTexture(this->renderer, path.c_str());
    if (!tex)
    {
        std::cerr << "[SDLTextureFactory] Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return std::make_shared<SDLTexture>(tex);
}
