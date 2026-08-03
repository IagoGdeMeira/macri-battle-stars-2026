#ifndef sdl_draw_font_command_handler_h
#define sdl_draw_font_command_handler_h

#include "SDLFont/SDLFont.h"
#include "engine/draw_commands/DrawFontCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <SDL_ttf.h>
#include <cmath>
#include <string>
#include <unordered_map>

class SDLDrawFontCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawFontCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}
    ~SDLDrawFontCommandHandler() override { for (auto& [_, texture] : this->textCache) if (texture) SDL_DestroyTexture(texture); }

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawFontCommand&>(command);
        if (!cmd.font || cmd.text.empty()) return;

        const SDLFont* sdlFont = dynamic_cast<const SDLFont*>(cmd.font);
        if (!sdlFont) return;

        int fontSize = static_cast<int>(std::lround(cmd.fontSize));
        TTF_Font* ttfFont = sdlFont->getFontWithSize(fontSize);
        if (!ttfFont) return;

        std::string cacheKey = cmd.text + "_" + std::to_string(fontSize) + "_" +
            std::to_string(cmd.color.r) + "," + std::to_string(cmd.color.g) + "," +
            std::to_string(cmd.color.b) + "," + std::to_string(cmd.color.a);

        SDL_Texture* texture = nullptr;
        auto it = this->textCache.find(cacheKey);

        if (it != this->textCache.end()) texture = it->second;
        else
        {
            SDL_Color sdlColor {
                static_cast<Uint8>(cmd.color.r),
                static_cast<Uint8>(cmd.color.g),
                static_cast<Uint8>(cmd.color.b),
                static_cast<Uint8>(cmd.color.a)
            };
            SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, cmd.text.c_str(), sdlColor);
            if (!surface) return;

            texture = SDL_CreateTextureFromSurface(this->renderer, surface);
            SDL_FreeSurface(surface);
            if (!texture) return;

            this->textCache[cacheKey] = texture;
        }

        SDL_Rect dst {
            static_cast<int>(std::lround(cmd.dest.position.x)),
            static_cast<int>(std::lround(cmd.dest.position.y)),
            static_cast<int>(std::lround(cmd.dest.size.width)),
            static_cast<int>(std::lround(cmd.dest.size.height))
        };
        SDL_RenderCopy(this->renderer, texture, nullptr, &dst);
    }

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, SDL_Texture*> textCache;
};

#endif // sdl_draw_font_command_handler_h
