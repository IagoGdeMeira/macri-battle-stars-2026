#include "../include/SDLRenderer/SDLRenderer.h"

#include "../include/SDLFont/SDLFont.h"
#include "../include/SDLTexture/SDLTexture.h"

#include "../../domain/utils/Logger/Logger.h"

#include <chrono>
#include <cmath>
#include <SDL_image.h>
#include <stdexcept>

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

    this->registerHandler<DrawTextureCommand>([this](const DrawTextureCommand& cmd) { this->drawTextureImpl(cmd); });
    this->registerHandler<DrawFontCommand>([this](const DrawFontCommand& cmd) { this->drawFontImpl(cmd); });
    this->registerHandler<DrawRectangleCommand>([this](const DrawRectangleCommand& cmd) { this->drawRectangleImpl(cmd); });
    this->registerHandler<DrawCircleCommand>([this](const DrawCircleCommand& cmd) { this->drawCircleImpl(cmd); });
}

void SDLRenderer::present()
{
    auto start = std::chrono::high_resolution_clock::now();
    SDL_RenderPresent(this->renderer);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration > 5) LOG_DEBUG("SDLRenderer::present took {} ms", duration);
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}

void SDLRenderer::drawTextureImpl(const DrawTextureCommand& cmd)
{
    auto startTotal = std::chrono::high_resolution_clock::now();

    if (!cmd.texture) return;
    const SDLTexture& sdlTex = static_cast<const SDLTexture&>(*cmd.texture);

    auto& dest = cmd.dest;
    SDL_Rect dst =
    {
        static_cast<int>(std::lround(dest.position.x)),
        static_cast<int>(std::lround(dest.position.y)),
        static_cast<int>(std::lround(dest.size.width)),
        static_cast<int>(std::lround(dest.size.height))
    };

    SDL_Rect* srcPtr = nullptr;
    SDL_Rect src;
    if (cmd.useSourceRect)
    {
        auto& source = cmd.source;
        src =
        {
            static_cast<int>(std::lround(source.position.x)),
            static_cast<int>(std::lround(source.position.y)),
            static_cast<int>(std::lround(source.size.width)),
            static_cast<int>(std::lround(source.size.height))
        };
        srcPtr = &src;
    }

    SDL_Point pivot =
    {
        static_cast<int>(cmd.pivot.x * dest.size.width),
        static_cast<int>(cmd.pivot.y * dest.size.height)
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (cmd.flipX) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (cmd.flipY) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    SDL_BlendMode sdlBlend = SDL_BLENDMODE_BLEND;
    switch (cmd.blend)
    {
        case BlendMode::Normal:   sdlBlend = SDL_BLENDMODE_BLEND; break;
        case BlendMode::Add:      sdlBlend = SDL_BLENDMODE_ADD; break;
        case BlendMode::Multiply: sdlBlend = SDL_BLENDMODE_MOD; break;
    }

    SDL_BlendMode oldBlend;
    SDL_GetTextureBlendMode(sdlTex.get(), &oldBlend);
    SDL_SetTextureBlendMode(sdlTex.get(), sdlBlend);

    auto copyStart = std::chrono::high_resolution_clock::now();

    if (cmd.tint != Color::WHITE())
    {
        Uint8 oldR, oldG, oldB;
        SDL_GetTextureColorMod(sdlTex.get(), &oldR, &oldG, &oldB);
        SDL_SetTextureColorMod(sdlTex.get(), cmd.tint.r, cmd.tint.g, cmd.tint.b);

        SDL_RenderCopyEx(this->renderer, sdlTex.get(), srcPtr, &dst, cmd.rotation, &pivot, flip);
        SDL_SetTextureColorMod(sdlTex.get(), oldR, oldG, oldB);
    }
    else SDL_RenderCopyEx(this->renderer, sdlTex.get(), srcPtr, &dst, cmd.rotation, &pivot, flip);

    SDL_SetTextureBlendMode(sdlTex.get(), oldBlend);

    auto copyEnd = std::chrono::high_resolution_clock::now();
    auto copyDuration = std::chrono::duration_cast<std::chrono::milliseconds>(copyEnd - copyStart).count();
    if (copyDuration > 5)
    {
        LOG_DEBUG("SDL_RenderCopyEx took {} ms", copyDuration);
    }

    auto endTotal = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTotal - startTotal).count();
    if (totalDuration > 10) LOG_DEBUG("drawTextureImpl total took {} ms", totalDuration);
}

void SDLRenderer::drawFontImpl(const DrawFontCommand& cmd)
{
    if (!cmd.font) return;
    if (!this->renderer) return;

    const SDLFont* sdlFont = dynamic_cast<const SDLFont*>(cmd.font);
    if (!sdlFont) return;

    TTF_Font* ttfFont = nullptr;
    try { ttfFont = sdlFont->getFontWithSize(cmd.fontSize); }
    catch (...) { return; }
    if (!ttfFont) return;

    if (TTF_WasInit() == 0) return;

    SDL_Color sdlColor = { cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a };
    SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, cmd.text.c_str(), sdlColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
    if (texture)
    {
        auto& dest = cmd.dest;
        SDL_Rect dst =
        {
            static_cast<int>(std::lround(dest.position.x)),
            static_cast<int>(std::lround(dest.position.y)),
            static_cast<int>(std::lround(dest.size.width)),
            static_cast<int>(std::lround(dest.size.height))
        };
        SDL_RenderCopy(this->renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void SDLRenderer::drawRectangleImpl(const DrawRectangleCommand& cmd)
{
    if (cmd.filled) this->drawRectFilled(cmd.rect, cmd.color);
    else this->drawRectOutline(cmd.rect, cmd.color);
}

void SDLRenderer::drawCircleImpl(const DrawCircleCommand& cmd)
{
    if (cmd.filled) this->drawCircleFilled(cmd.circle, cmd.color);
    else this->drawCircleOutline(cmd.circle, cmd.color);
}

void SDLRenderer::drawRectOutline(const Rectangle& rect, const Color& color)
{
    SDL_SetRenderDrawColor(this->renderer, color.r, color.g, color.b, color.a);
    SDL_Rect sdlRect =
    {
        static_cast<int>(std::lround(rect.position.x)),
        static_cast<int>(std::lround(rect.position.y)),
        static_cast<int>(std::lround(rect.size.width)),
        static_cast<int>(std::lround(rect.size.height))
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
        static_cast<int>(std::lround(rect.size.width)),
        static_cast<int>(std::lround(rect.size.height))
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
        int dx = static_cast<int>(std::lround(std::sqrt(r * r - dy * dy)));
        SDL_RenderDrawLine(this->renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}
