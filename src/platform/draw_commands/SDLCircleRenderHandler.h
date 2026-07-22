#ifndef sdl_circle_render_handler_h
#define sdl_circle_render_handler_h

#include "engine/draw_commands/DrawCircleCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <cmath>
#include <SDL.h>

class SDLCircleRenderHandler : public IDrawCommandHandler
{
public:
    explicit SDLCircleRenderHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawCircleCommand&>(command);
        int cx = static_cast<int>(std::lround(cmd.circle.position.x));
        int cy = static_cast<int>(std::lround(cmd.circle.position.y));
        int r = static_cast<int>(std::lround(cmd.circle.radius));

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
    }

private:
    SDL_Renderer* renderer;
};

#endif // sdl_circle_render_handler_h
