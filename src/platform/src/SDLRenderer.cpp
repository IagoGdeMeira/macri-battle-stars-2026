#include "SDLRenderer/SDLRenderer.h"

#include "SDLFont/SDLFont.h"
#include "SDLTexture/SDLTexture.h"

#include "domain/utils/Logger/Logger.h"

#include <chrono>
#include <cmath>
#include <SDL_image.h>
#include <stdexcept>

SDLRenderer::SDLRenderer(SDL_Window* window)
{
    // this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
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
    if (!this->renderer)
    {
        LOG_ERROR("SDLRenderer::present called with null renderer");
        return;
    }

    LOG_DEBUG("SDLRenderer::present: before SDL_RenderPresent");
    auto start = std::chrono::high_resolution_clock::now();
    SDL_RenderPresent(this->renderer);
    auto end = std::chrono::high_resolution_clock::now();
    LOG_DEBUG("SDLRenderer::present: after SDL_RenderPresent");
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (duration > 5) LOG_DEBUG("SDLRenderer::present took {} ms", duration);

    const char* error = SDL_GetError();
    if (error && error[0]) LOG_ERROR("SDL error after present: {}", error);
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
}

void SDLRenderer::drawTextureImpl(const DrawTextureCommand& cmd)
{
    LOG_DEBUG("drawTextureImpl: start for texture");

    using hrclock = std::chrono::high_resolution_clock;
    using ms = std::chrono::milliseconds;

    auto startTotal = hrclock::now();

    if (!this->renderer)
    {
        LOG_ERROR("drawTextureImpl: renderer is null!");
        return;
    }

    if (!cmd.texture)
    {
        LOG_DEBUG("drawTextureImpl: texture is null, returning");
        return;
    }
    LOG_DEBUG("drawTextureImpl: texture pointer OK");

    const SDLTexture& sdlTex = static_cast<const SDLTexture&>(*cmd.texture);
    SDL_Texture* sdlTexturePtr = sdlTex.get();
    LOG_DEBUG("drawTextureImpl: SDL_Texture ptr = {}", (void*)sdlTexturePtr);

    int texW = 0, texH = 0;
    if (SDL_QueryTexture(sdlTexturePtr, nullptr, nullptr, &texW, &texH) == 0) LOG_DEBUG("drawTextureImpl: texture dimensions = {}x{}", texW, texH);
    else LOG_ERROR("drawTextureImpl: SDL_QueryTexture failed: {}", SDL_GetError());

    auto& dest = cmd.dest;
    SDL_Rect dst =
    {
        static_cast<int>(std::lround(dest.position.x)),
        static_cast<int>(std::lround(dest.position.y)),
        static_cast<int>(std::lround(dest.size.width)),
        static_cast<int>(std::lround(dest.size.height))
    };
    LOG_DEBUG("drawTextureImpl: dst = ({},{},{},{})", dst.x, dst.y, dst.w, dst.h);

    SDL_Rect* srcPtr = nullptr;
    SDL_Rect src = {0, 0, 0, 0};
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
        LOG_DEBUG("drawTextureImpl: src = ({},{},{},{})", src.x, src.y, src.w, src.h);
    }
    else LOG_DEBUG("drawTextureImpl: no source rect (using full texture)");
    
    if (srcPtr && texW > 0 && texH > 0 && (src.x + src.w > texW || src.y + src.h > texH)) LOG_WARN(
        "drawTextureImpl: source rect out of texture bounds! src=({},{},{},{}) texture={}x{}",
        src.x, src.y, src.w, src.h, texW, texH);

    SDL_Point pivot =
    {
        static_cast<int>(cmd.pivot.x * dest.size.width),
        static_cast<int>(cmd.pivot.y * dest.size.height)
    };
    LOG_DEBUG("drawTextureImpl: pivot = ({},{}) (normalized: {},{})", pivot.x, pivot.y, cmd.pivot.x, cmd.pivot.y);

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (cmd.flipX) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (cmd.flipY) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
    LOG_DEBUG("drawTextureImpl: flip = {}", static_cast<int>(flip));

    SDL_BlendMode sdlBlend = SDL_BLENDMODE_BLEND;
    switch (cmd.blend)
    {
        case BlendMode::Normal:   sdlBlend = SDL_BLENDMODE_BLEND; break;
        case BlendMode::Add:      sdlBlend = SDL_BLENDMODE_ADD; break;
        case BlendMode::Multiply: sdlBlend = SDL_BLENDMODE_MOD; break;
        default: break;
    }
    LOG_DEBUG("drawTextureImpl: blend mode = {}", static_cast<int>(sdlBlend));

    SDL_BlendMode oldBlend;
    SDL_GetTextureBlendMode(sdlTexturePtr, &oldBlend);
    SDL_SetTextureBlendMode(sdlTexturePtr, sdlBlend);

    auto copyStart = hrclock::now();

    int result = -1;
    if (cmd.tint != Color::WHITE())
    {
        Uint8 oldR, oldG, oldB;
        SDL_GetTextureColorMod(sdlTexturePtr, &oldR, &oldG, &oldB);
        SDL_SetTextureColorMod(sdlTexturePtr, cmd.tint.r, cmd.tint.g, cmd.tint.b);

        LOG_DEBUG("drawTextureImpl: calling SDL_RenderCopyEx with tint ({},{},{})", cmd.tint.r, cmd.tint.g, cmd.tint.b);
        result = SDL_RenderCopyEx(this->renderer, sdlTexturePtr, srcPtr, &dst, cmd.rotation, &pivot, flip);

        SDL_SetTextureColorMod(sdlTexturePtr, oldR, oldG, oldB);
    }
    else
    {
        LOG_DEBUG("drawTextureImpl: calling SDL_RenderCopyEx (no tint)");
        result = SDL_RenderCopyEx(this->renderer, sdlTexturePtr, srcPtr, &dst, cmd.rotation, &pivot, flip);
    }

    SDL_SetTextureBlendMode(sdlTexturePtr, oldBlend);

    if (result == 0) LOG_DEBUG("drawTextureImpl: SDL_RenderCopyEx succeeded"); 
    else
    {
        const char* error = SDL_GetError();
        LOG_ERROR("drawTextureImpl: SDL_RenderCopyEx failed with code {}: {}", result, error ? error : "unknown error");
    }

    LOG_DEBUG("drawTextureImpl: after SDL_RenderCopyEx (function returned)");

    auto copyEnd = hrclock::now();
    auto copyDuration = std::chrono::duration_cast<ms>(copyEnd - copyStart).count();
    if (copyDuration > 5) LOG_DEBUG("SDL_RenderCopyEx took {} ms", copyDuration);

    auto endTotal = hrclock::now();
    auto totalDuration = std::chrono::duration_cast<ms>(endTotal - startTotal).count();
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
