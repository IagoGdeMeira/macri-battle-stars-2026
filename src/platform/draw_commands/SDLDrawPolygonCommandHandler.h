#ifndef sdl_draw_polygon_command_handler_h
#define sdl_draw_polygon_command_handler_h

#include "engine/draw_commands/DrawPolygonCommand.h"
#include "engine/include/IDrawCommandHandler/IDrawCommandHandler.h"

#include <cmath>
#include <vector>
#include <SDL.h>

class SDLDrawPolygonCommandHandler : public IDrawCommandHandler
{
public:
    explicit SDLDrawPolygonCommandHandler(SDL_Renderer* renderer) : renderer(renderer) {}

    void execute(const DrawCommand& command) override
    {
        const auto& cmd = static_cast<const DrawPolygonCommand&>(command);
        if (cmd.polygon.vertices.size() < 3) return;

        this->applyBlendMode(cmd.blend);

        if (cmd.filled) this->drawFilled(cmd);
        else this->drawOutline(cmd);

        SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_NONE);
    }

private:
    SDL_Renderer* renderer;

    void drawFilled(const DrawPolygonCommand& cmd)
    {
        const auto& verts = cmd.polygon.vertices;
        std::vector<SDL_Vertex> vertices;
        vertices.reserve(verts.size());

        for (const auto& v : verts)
        {
            SDL_Vertex sv;
            sv.position.x = v.x;
            sv.position.y = v.y;
            sv.color.r = cmd.color.r;
            sv.color.g = cmd.color.g;
            sv.color.b = cmd.color.b;
            sv.color.a = cmd.color.a;
            sv.tex_coord.x = 0.f;
            sv.tex_coord.y = 0.f;
            vertices.push_back(sv);
        }

        std::vector<int> indices;
        for (int i = 1; i + 1 < static_cast<int>(vertices.size()); ++i)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        SDL_RenderGeometry(this->renderer, nullptr, vertices.data(),
            static_cast<int>(vertices.size()), indices.data(),
            static_cast<int>(indices.size()));
    }

    void drawOutline(const DrawPolygonCommand& cmd)
    {
        const auto& verts = cmd.polygon.vertices;
        SDL_SetRenderDrawColor(this->renderer, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);

        for (size_t i = 0; i < verts.size(); ++i)
        {
            const auto& a = verts[i];
            const auto& b = verts[(i + 1) % verts.size()];
            SDL_RenderDrawLine(this->renderer,
                static_cast<int>(std::lround(a.x)), static_cast<int>(std::lround(a.y)),
                static_cast<int>(std::lround(b.x)), static_cast<int>(std::lround(b.y)));
        }
    }

    void applyBlendMode(BlendMode blend)
    {
        SDL_BlendMode sdlBlend = SDL_BLENDMODE_NONE;
        switch (blend)
        {
            case BlendMode::Normal:     sdlBlend = SDL_BLENDMODE_BLEND; break;
            case BlendMode::Add:        sdlBlend = SDL_BLENDMODE_ADD;   break;
            case BlendMode::Multiply:   sdlBlend = SDL_BLENDMODE_MOD;   break;
            default: break;
        }
        SDL_SetRenderDrawBlendMode(this->renderer, sdlBlend);
    }
};

#endif // sdl_draw_polygon_command_handler_h
