#include "SDLRenderer/SDLRenderer.h"

#include "domain/utils/Logger/Logger.h"

#include <SDL_image.h>

SDLRenderer::SDLRenderer(SDL_Window* window)
{
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!this->renderer) this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    
    SDL_RendererInfo info;
    SDL_GetRendererInfo(this->renderer, &info);

    this->registerHandler<DrawTextureCommand>(std::make_unique<SDLDrawTextureCommandHandler>(this->renderer));
    this->registerHandler<DrawFontCommand>(std::make_unique<SDLDrawFontCommandHandler>(this->renderer));
    this->registerHandler<DrawRectangleCommand>(std::make_unique<SDLDrawRectangleCommandHandler>(this->renderer));
    this->registerHandler<DrawCircleCommand>(std::make_unique<SDLDrawCircleCommandHandler>(this->renderer));
}

SDLRenderer::~SDLRenderer() { if (this->renderer) SDL_DestroyRenderer(this->renderer); }

void SDLRenderer::clear()
{
    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->renderer);
}

void SDLRenderer::present() { SDL_RenderPresent(this->renderer); }

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}

void SDLRenderer::setScale(const Position& scale) { SDL_RenderSetScale(this->renderer, scale.x, scale.y); }

void SDLRenderer::draw(const DrawCommand& command)
{
    auto it = this->handlers.find(command.type());
    if (it != this->handlers.end()) it->second->execute(command);
}
