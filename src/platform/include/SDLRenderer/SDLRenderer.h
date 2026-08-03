#ifndef sdl_renderer_h
#define sdl_renderer_h

#include "engine/include/Renderer/Renderer.h"

#include <SDL.h>

class SDLRenderer : public Renderer
{
public:
    explicit SDLRenderer(SDL_Window* window);
    ~SDLRenderer() override;

    void clear() override;
    void present() override;
    void setViewport(const Viewport& viewport) override;
    void setScale(const Position& scale) override;

    SDL_Renderer* get() const { return this->renderer; }

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
