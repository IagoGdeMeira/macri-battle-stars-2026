#include "../include/SDLRenderer/SDLRenderer.h"

#include "../include/SDLTexture/SDLTexture.h"

#include <SDL_image.h>
#include <stdexcept>

SDLRenderer::SDLRenderer(SDL_Window* window)
{ this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); }

SDLRenderer::~SDLRenderer()
{ if (this->renderer) SDL_DestroyRenderer(this->renderer); }

void SDLRenderer::clear() { SDL_RenderClear(this->renderer); }

void SDLRenderer::present() { SDL_RenderPresent(this->renderer); }

std::shared_ptr<Texture> SDLRenderer::createTexture(const std::string& path)
{
    SDL_Texture* tex = IMG_LoadTexture(this->renderer, path.c_str());
    if (!tex) return nullptr;

    return std::make_shared<SDLTexture>(tex);
}

void SDLRenderer::draw(const Texture& texture, int x, int y, int width, int height)
{
    const SDLTexture& sdlTexture = static_cast<const SDLTexture&>(texture);

    SDL_Rect dst = { x, y, width, height };
    SDL_RenderCopy(this->renderer, sdlTexture.get(), nullptr, &dst);
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}
