#include "SDLTextureFactory/SDLTextureFactory.h"

#include "SDLTexture/SDLTexture.h"

#include "engine/include/PathResolver/PathResolver.h"

#include <iostream>
#include <SDL_image.h>

std::shared_ptr<Texture> SDLTextureFactory::createTexture(const std::string& path)
{
    const auto resolvedPath = PathResolver::resolve(path);
    SDL_Texture* tex = IMG_LoadTexture(this->renderer, resolvedPath.string().c_str());
    if (!tex)
    {
        std::cerr << "[SDLTextureFactory] Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return std::make_shared<SDLTexture>(tex);
}
