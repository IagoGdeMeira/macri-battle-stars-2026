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
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) throw std::runtime_error("Failed to load image: " + path);

    SDL_Texture* tex = SDL_CreateTextureFromSurface(this->renderer, surface);
    SDL_FreeSurface(surface);

    if (!tex) throw std::runtime_error("Failed to create texture: " + path);
    return std::make_shared<SDLTexture>(tex);
}

void SDLRenderer::draw(const Texture& texture, int x, int y, int width, int height)
{
    const SDLTexture& sdlTexture = dynamic_cast<const SDLTexture&>(texture);
    SDL_Rect rect = { x, y, width, height };

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(this->renderer, &rect);
}

void SDLRenderer::setViewport(int x, int y, int width, int height)
{
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderSetViewport(this->renderer, &rect);
}
