#ifndef sdl_draw_rectangle_command_handler_h
#define sdl_draw_rectangle_command_handler_h

#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <cmath>
#include <SDL.h>

class SDLDrawRectangleCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawRectangleCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawRectangleCommand&>(command);
        this->applyBlendMode(cmd.blend);

        SDL_Rect rect {
            static_cast<int>(std::lround(cmd.rect.position.x)),
            static_cast<int>(std::lround(cmd.rect.position.y)),
            static_cast<int>(std::lround(cmd.rect.size.width)),
            static_cast<int>(std::lround(cmd.rect.size.height))
        };
        SDL_SetRenderDrawColor(this->renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
        if (cmd.filled) SDL_RenderFillRect(this->renderer, &rect);
        else SDL_RenderDrawRect(this->renderer, &rect);

        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_NONE);
    }

private:
    SDL_Renderer* renderer;

    void applyBlendMode(BlendMode blend)
    {
        SDL_BlendMode sdlBlend = SDL_BLENDMODE_NONE;
        switch (blend)
        {
            case BlendMode::Normal:     sdlBlend = SDL_BLENDMODE_BLEND; break;
            case BlendMode::Add:        sdlBlend = SDL_BLENDMODE_ADD;    break;
            case BlendMode::Multiply:   sdlBlend = SDL_BLENDMODE_MOD;    break;
            default: break;
        }
        SDL_SetRenderDrawBlendMode(this->renderer, sdlBlend);
    }
};

#endif // sdl_draw_rectangle_command_handler_h
