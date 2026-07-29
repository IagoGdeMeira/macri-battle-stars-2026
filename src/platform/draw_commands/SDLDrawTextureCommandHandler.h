#ifndef sdl_draw_texture_command_handler_h
#define sdl_draw_texture_command_handler_h

#include "SDLTexture/SDLTexture.h"
#include "engine/draw_commands/DrawTextureCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"
#include <cmath>
#include <SDL.h>

class SDLDrawTextureCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawTextureCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawTextureCommand&>(command);
        if (!cmd.texture) return;

        const SDLTexture& sdlTex = static_cast<const SDLTexture&>(*cmd.texture);
        SDL_Texture* tex = sdlTex.get();
        if (!tex) return;

        SDL_Rect dst = this->buildDestRect(cmd);
        SDL_Rect srcRect;
        SDL_Rect* srcPtr = this->buildSourceRect(cmd, srcRect);
        SDL_Point pivot = this->calculatePivot(cmd);
        SDL_RendererFlip flip = this->calculateFlip(cmd);

        SDL_BlendMode oldBlend = this->applyBlendMode(tex, cmd.blend);

        if (cmd.tint != Color::WHITE()) this->applyColorMod(tex, cmd.tint, [&]()
        { SDL_RenderCopyEx(this->renderer, tex, srcPtr, &dst, cmd.rotation, &pivot, flip); });
        else SDL_RenderCopyEx(this->renderer, tex, srcPtr, &dst, cmd.rotation, &pivot, flip);

        SDL_SetTextureBlendMode(tex, oldBlend);
    }

private:
    SDL_Renderer* renderer;

    SDL_Rect buildDestRect(const DrawTextureCommand& cmd) const
    {
        return {
            static_cast<int>(std::lround(cmd.dest.position.x)),
            static_cast<int>(std::lround(cmd.dest.position.y)),
            static_cast<int>(std::lround(cmd.dest.size.width)),
            static_cast<int>(std::lround(cmd.dest.size.height))
        };
    }

    SDL_Rect* buildSourceRect(const DrawTextureCommand& cmd, SDL_Rect& outSrc) const
    {
        if (!cmd.useSourceRect) return nullptr;
        outSrc = {
            static_cast<int>(std::lround(cmd.source.position.x)),
            static_cast<int>(std::lround(cmd.source.position.y)),
            static_cast<int>(std::lround(cmd.source.size.width)),
            static_cast<int>(std::lround(cmd.source.size.height))
        };
        return &outSrc;
    }

    SDL_Point calculatePivot(const DrawTextureCommand& cmd) const
    {
        return {
            static_cast<int>(cmd.pivot.x * cmd.dest.size.width),
            static_cast<int>(cmd.pivot.y * cmd.dest.size.height)
        };
    }

    SDL_RendererFlip calculateFlip(const DrawTextureCommand& cmd) const
    {
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (cmd.flipX) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
        if (cmd.flipY) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
        return flip;
    }

    SDL_BlendMode applyBlendMode(SDL_Texture* tex, BlendMode blend) const
    {
        SDL_BlendMode oldBlend;
        SDL_GetTextureBlendMode(tex, &oldBlend);
        SDL_BlendMode sdlBlend = SDL_BLENDMODE_BLEND;
        switch (blend)
        {
            case BlendMode::Normal:     sdlBlend = SDL_BLENDMODE_BLEND; break;
            case BlendMode::Add:        sdlBlend = SDL_BLENDMODE_ADD;    break;
            case BlendMode::Multiply:   sdlBlend = SDL_BLENDMODE_MOD;    break;
            default: break;
        }
        SDL_SetTextureBlendMode(tex, sdlBlend);
        return oldBlend;
    }

    template <typename Func>
    void applyColorMod(SDL_Texture* tex, const Color& tint, Func drawFunc) const
    {
        Uint8 oldR, oldG, oldB;
        SDL_GetTextureColorMod(tex, &oldR, &oldG, &oldB);
        SDL_SetTextureColorMod(tex, tint.r, tint.g, tint.b);
        drawFunc();
        SDL_SetTextureColorMod(tex, oldR, oldG, oldB);
    }
};

#endif // sdl_draw_texture_command_handler_h
