#include "../include/SDLRenderer/SDLRenderer.h"

SDLRenderer::SDLRenderer(SDL_Window* window)
{ this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); }

SDLRenderer::~SDLRenderer()
{ if (this->renderer) SDL_DestroyRenderer(this->renderer); }

void SDLRenderer::clear() { SDL_RenderClear(this->renderer); }

void SDLRenderer::present() { SDL_RenderPresent(this->renderer); }

void SDLRenderer::draw(const std::string&, int x, int y, int width, int height)
{
    SDL_Rect rect = { x, y, width, height };

    SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(this->renderer, &rect);
}

void SDLRenderer::setViewport(int x, int y, int width, int height)
{
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderSetViewport(this->renderer, &rect);
}
