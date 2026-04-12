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

void SDLRenderer::draw(const Texture& texture, const DrawParams& params)
{
    auto& sdlTex = static_cast<const SDLTexture&>(texture);

    SDL_Rect dst = { params.x, params.y, params.width, params.height };
    
    SDL_Rect* srcPtr = nullptr;
    SDL_Rect src;
    if (params.useSourceRect)
    {
        src = { params.srcX, params.srcY, params.srcWidth, params.srcHeight };
        srcPtr = &src;
    }

    SDL_Point pivot =
    {
        static_cast<int>(params.pivotX * params.width),
        static_cast<int>(params.pivotY * params.height)
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (params.flipX) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (params.flipY) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    SDL_RenderCopyEx(
        this->renderer,
        sdlTex.get(),
        srcPtr,
        &dst,
        params.rotation,
        &pivot,
        flip);
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}
