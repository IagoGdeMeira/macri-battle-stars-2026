#ifndef sdl_rectangle_render_handler_h
#define sdl_rectangle_render_handler_h

#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <cmath>
#include <SDL.h>

class SDLRectangleRenderHandler : public IDrawCommandHandler
{
public:
    explicit SDLRectangleRenderHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawRectangleCommand&>(command);
        SDL_Rect rect {
            static_cast<int>(std::lround(cmd.rect.position.x)),
            static_cast<int>(std::lround(cmd.rect.position.y)),
            static_cast<int>(std::lround(cmd.rect.size.width)),
            static_cast<int>(std::lround(cmd.rect.size.height))
        };
        SDL_SetRenderDrawColor(this->renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
        if (cmd.filled) SDL_RenderFillRect(this->renderer, &rect);
        else SDL_RenderDrawRect(this->renderer, &rect);
    }

private:
    SDL_Renderer* renderer;
};

#endif // sdl_rectangle_render_handler_h
