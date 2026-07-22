#include "SDLRenderer/SDLRenderer.h"

#include "domain/utils/Logger/Logger.h"

#include <SDL_image.h>

SDLRenderer::SDLRenderer(SDL_Window* window)
{
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!this->renderer)
    {
        LOG_WARN("SDL_CreateRenderer with ACCELERATED failed, falling back to SOFTWARE");
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    SDL_RendererInfo info;
    SDL_GetRendererInfo(this->renderer, &info);
    LOG_DEBUG("SDLRenderer: using driver '{}'", info.name);

    this->registerHandler<DrawTextureCommand>(std::make_unique<SDLTextureRenderHandler>(this->renderer));
    this->registerHandler<DrawFontCommand>(std::make_unique<SDLFontRenderHandler>(this->renderer));
    this->registerHandler<DrawRectangleCommand>(std::make_unique<SDLRectangleRenderHandler>(this->renderer));
    this->registerHandler<DrawCircleCommand>(std::make_unique<SDLCircleRenderHandler>(this->renderer));
}

SDLRenderer::~SDLRenderer() { if (this->renderer) SDL_DestroyRenderer(this->renderer); }

void SDLRenderer::clear() { SDL_RenderClear(this->renderer); }

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
    else LOG_ERROR("No handler registered for command type");
}
