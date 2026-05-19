#include "../include/SDLRenderer/SDLRenderer.h"

#include "../include/SDLFont/SDLFont.h"
#include "../include/SDLTexture/SDLTexture.h"

#include <cmath>
#include <SDL_image.h>
#include <stdexcept>

SDLRenderer::SDLRenderer(SDL_Window* window)
{
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SDLRenderer::~SDLRenderer()
{
    if (this->renderer) SDL_DestroyRenderer(this->renderer);
}

void SDLRenderer::clear() { SDL_RenderClear(this->renderer); }

void SDLRenderer::present() { SDL_RenderPresent(this->renderer); }

void SDLRenderer::drawTexture(const DrawTextureCommand& cmd)
{
    if (!cmd.texture) return;
    const SDLTexture& sdlTex = static_cast<const SDLTexture&>(*cmd.texture);

    SDL_Rect dst =
    {
        static_cast<int>(std::lround(cmd.dest.position.x)),
        static_cast<int>(std::lround(cmd.dest.position.y)),
        static_cast<int>(std::lround(cmd.dest.size.width)),
        static_cast<int>(std::lround(cmd.dest.size.height))
    };
    
    SDL_Rect* srcPtr = nullptr;
    SDL_Rect src;
    if (cmd.useSourceRect)
    {
        src =
        {
            static_cast<int>(std::lround(cmd.source.position.x)),
            static_cast<int>(std::lround(cmd.source.position.y)),
            static_cast<int>(std::lround(cmd.source.size.width)),
            static_cast<int>(std::lround(cmd.source.size.height))
        };
        srcPtr = &src;
    }

    SDL_Point pivot =
    {
        static_cast<int>(cmd.pivot.x * cmd.dest.size.width),
        static_cast<int>(cmd.pivot.y * cmd.dest.size.height)
    };

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (cmd.flipX) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (cmd.flipY) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    SDL_BlendMode sdlBlend = SDL_BLENDMODE_BLEND;
    switch (cmd.blend)
    {
        case BlendMode::Normal: sdlBlend = SDL_BLENDMODE_BLEND; break;
        case BlendMode::Add: sdlBlend = SDL_BLENDMODE_ADD; break;
        case BlendMode::Multiply: sdlBlend = SDL_BLENDMODE_MOD; break;
    }

    SDL_BlendMode oldBlend;
    SDL_GetTextureBlendMode(sdlTex.get(), &oldBlend);
    SDL_SetTextureBlendMode(sdlTex.get(), sdlBlend);

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
}

void SDLRenderer::drawFont(const DrawFontCommand& cmd)
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
        SDL_Rect dst =
        {
            static_cast<int>(std::lround(cmd.dest.position.x)),
            static_cast<int>(std::lround(cmd.dest.position.y)),
            static_cast<int>(std::lround(cmd.dest.size.width)),
            static_cast<int>(std::lround(cmd.dest.size.height))
        };
        SDL_RenderCopy(this->renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void SDLRenderer::drawRectangle(const DrawRectangleCommand& cmd)
{
    if (cmd.filled) this->drawRectFilled(cmd.rect, cmd.color);
    else this->drawRectOutline(cmd.rect, cmd.color);
}

void SDLRenderer::drawCircle(const DrawCircleCommand& cmd)
{
    if (cmd.filled) this->drawCircleFilled(cmd.circle, cmd.color);
    else this->drawCircleOutline(cmd.circle, cmd.color);
}

void SDLRenderer::setViewport(const Viewport& viewport)
{
    SDL_Rect rect = { viewport.x, viewport.y, viewport.width, viewport.height };
    SDL_RenderSetViewport(this->renderer, &rect);
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
