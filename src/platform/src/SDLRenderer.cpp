#include "../include/SDLRenderer/SDLRenderer.h"

#include "../include/SDLFont/SDLFont.h"
#include "../include/SDLTexture/SDLTexture.h"

#include <cmath>
#include <SDL_image.h>
#include <stdexcept>

SDLRenderer::SDLRenderer(SDL_Window* window)
{ this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); }

SDLRenderer::~SDLRenderer()
{ if (this->renderer) SDL_DestroyRenderer(this->renderer); }

void SDLRenderer::clear() { SDL_RenderClear(this->renderer); }

void SDLRenderer::present() { SDL_RenderPresent(this->renderer); }

void SDLRenderer::drawTexture(const Texture& texture, const DrawTextureParams& params)
{
    auto& sdlTex = static_cast<const SDLTexture&>(texture);

    SDL_Rect dst =
    {
        static_cast<int>(std::lround(params.dest.position.x)),
        static_cast<int>(std::lround(params.dest.position.y)),
        static_cast<int>(std::lround(params.dest.width)),
        static_cast<int>(std::lround(params.dest.height))
    };
    
    SDL_Rect* srcPtr = nullptr;
    SDL_Rect src;
    if (params.useSourceRect)
    {
        src =
        {
            static_cast<int>(std::lround(params.source.position.x)),
            static_cast<int>(std::lround(params.source.position.y)),
            static_cast<int>(std::lround(params.source.width)),
            static_cast<int>(std::lround(params.source.height))
        };
        srcPtr = &src;
    }

    SDL_Point pivot =
    {
        static_cast<int>(params.pivotX * params.dest.width),
        static_cast<int>(params.pivotY * params.dest.height)
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

void SDLRenderer::drawText(const Font& font, const DrawTextParams& params)
{
    const auto& sdlFont = static_cast<const SDLFont&>(font);
    TTF_Font* ttfFont = sdlFont.getFontWithSize(params.fontSize);
    if (!ttfFont) return;

    SDL_Color sdlColor = { params.color.r, params.color.g, params.color.b, params.color.a };
    SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, params.text.c_str(), sdlColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
    if (texture)
    {
        SDL_Rect dst =
        {
            static_cast<int>(std::lround(params.dest.position.x)),
            static_cast<int>(std::lround(params.dest.position.y)),
            static_cast<int>(std::lround(params.dest.width)),
            static_cast<int>(std::lround(params.dest.height))
        };
        SDL_RenderCopyEx(this->renderer, texture, nullptr, &dst, 0.0, nullptr, SDL_FLIP_NONE);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void SDLRenderer::drawRectOutline(const Rectangle& rect, const Color& color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdlRect =
    {
        static_cast<int>(std::lround(rect.position.x)),
        static_cast<int>(std::lround(rect.position.y)),
        static_cast<int>(std::lround(rect.width)),
        static_cast<int>(std::lround(rect.height))
    };
    SDL_RenderDrawRect(this->renderer, &sdlRect);
}

void SDLRenderer::drawRectFilled(const Rectangle& rect, const Color& color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdlRect =
    {
        static_cast<int>(std::lround(rect.position.x)),
        static_cast<int>(std::lround(rect.position.y)),
        static_cast<int>(std::lround(rect.width)),
        static_cast<int>(std::lround(rect.height))
    };
    SDL_RenderFillRect(this->renderer, &sdlRect);
}

void SDLRenderer::drawCircleOutline(const Circle& circle, const Color& color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    int cx = static_cast<int>(std::lround(circle.position.x));
    int cy = static_cast<int>(std::lround(circle.position.y));
    int r = static_cast<int>(std::lround(circle.radius));

    int x = 0, y = r;
    int d = 1 - r;
    while (x <= y)
    {
        SDL_RenderDrawPoint(this->renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(this->renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(this->renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(this->renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(this->renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(this->renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(this->renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(this->renderer, cx - y, cy - x);

        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; y--; }
        x++;
    }
}

void SDLRenderer::drawCircleFilled(const Circle& circle, const Color& color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    int cx = static_cast<int>(std::lround(circle.position.x));
    int cy = static_cast<int>(std::lround(circle.position.y));
    int r = static_cast<int>(std::lround(circle.radius));

    for (int dy = -r; dy <= r; dy++)
    {
        int dx = static_cast<int>(std::sqrt(r * r - dy * dy));
        SDL_RenderDrawLine(this->renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}
