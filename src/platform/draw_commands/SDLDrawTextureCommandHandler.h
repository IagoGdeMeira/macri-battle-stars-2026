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

        SDL_Rect dst {
            static_cast<int>(std::lround(cmd.dest.position.x)),
            static_cast<int>(std::lround(cmd.dest.position.y)),
            static_cast<int>(std::lround(cmd.dest.size.width)),
            static_cast<int>(std::lround(cmd.dest.size.height))
        };

        SDL_Rect src;
        SDL_Rect* srcPtr = nullptr;
        if (cmd.useSourceRect)
        {
            src = {
                static_cast<int>(std::lround(cmd.source.position.x)),
                static_cast<int>(std::lround(cmd.source.position.y)),
                static_cast<int>(std::lround(cmd.source.size.width)),
                static_cast<int>(std::lround(cmd.source.size.height))
            };
            srcPtr = &src;
        }

        SDL_Point pivot {
            static_cast<int>(cmd.pivot.x * cmd.dest.size.width),
            static_cast<int>(cmd.pivot.y * cmd.dest.size.height)
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
            default: break;
        }

        SDL_BlendMode oldBlend;
        SDL_GetTextureBlendMode(tex, &oldBlend);
        SDL_SetTextureBlendMode(tex, sdlBlend);

        if (cmd.tint != Color::WHITE())
        {
            Uint8 oldR, oldG, oldB;
            SDL_GetTextureColorMod(tex, &oldR, &oldG, &oldB);
            SDL_SetTextureColorMod(tex, cmd.tint.r, cmd.tint.g, cmd.tint.b);
            SDL_RenderCopyEx(this->renderer, tex, srcPtr, &dst, cmd.rotation, &pivot, flip);
            SDL_SetTextureColorMod(tex, oldR, oldG, oldB);
        }
        else SDL_RenderCopyEx(this->renderer, tex, srcPtr, &dst, cmd.rotation, &pivot, flip);

        SDL_SetTextureBlendMode(tex, oldBlend);
    }

private:
    SDL_Renderer* renderer;
};

#endif // sdl_draw_texture_command_handler_h
