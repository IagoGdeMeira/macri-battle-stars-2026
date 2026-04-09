#ifndef sdl_renderer_h
#define sdl_renderer_h

#include "../../engine/include/Renderer/Renderer.h"

#include <SDL.h>

class SDLRenderer : public Renderer
{
public:
    SDLRenderer(SDL_Window* window);
    ~SDLRenderer();

    void clear() override;
    void present() override;

    void draw(
        const std::string& textureId,
        int x, int y, int width, int height
    ) override;

    void setViewport(int x, int y, int width, int height) override;

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
