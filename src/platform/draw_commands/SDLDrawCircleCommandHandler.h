#ifndef sdl_draw_circle_command_handler_h
#define sdl_draw_circle_command_handler_h

#include "engine/draw_commands/DrawCircleCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <cmath>
#include <SDL.h>

class SDLDrawCircleCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawCircleCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawCircleCommand&>(command);
        this->applyBlendMode(cmd.blend);

        int cx = static_cast<int>(std::lround(cmd.circle.position.x));
        int cy = static_cast<int>(std::lround(cmd.circle.position.y));
        int r  = static_cast<int>(std::lround(cmd.circle.radius));

        SDL_SetRenderDrawColor(this->renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);

        if (cmd.filled) for (int dy = -r; dy <= r; ++dy)
        {
            int dx = static_cast<int>(std::lround(std::sqrt(r * r - dy * dy)));
            SDL_RenderDrawLine(this->renderer, cx - dx, cy + dy, cx + dx, cy + dy);
        }
        else
        {
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
                else { d += 2 * (x - y) + 5; --y; }
                ++x;
            }
        }

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

#endif // sdl_draw_circle_command_handler_h
