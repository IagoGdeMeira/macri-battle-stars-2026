#ifndef sdl_draw_font_command_handler_h
#define sdl_draw_font_command_handler_h

#include "SDLFont/SDLFont.h"

#include "engine/draw_commands/DrawFontCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <SDL_ttf.h>
#include <cmath>

class SDLDrawFontCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawFontCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawFontCommand&>(command);
        if (!cmd.font) return;

        const SDLFont* sdlFont = dynamic_cast<const SDLFont*>(cmd.font);
        if (!sdlFont) return;

        TTF_Font* ttfFont = sdlFont->getFontWithSize(static_cast<int>(std::lround(cmd.fontSize)));
        if (!ttfFont) return;

        SDL_Color sdlColor {
            static_cast<Uint8>(std::lround(cmd.color.r)),
            static_cast<Uint8>(std::lround(cmd.color.g)),
            static_cast<Uint8>(std::lround(cmd.color.b)),
            static_cast<Uint8>(std::lround(cmd.color.a))
        };
        SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, cmd.text.c_str(), sdlColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
        if (texture)
        {
            SDL_Rect dst {
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

private:
    SDL_Renderer* renderer;
};

#endif // sdl_draw_font_command_handler_h
