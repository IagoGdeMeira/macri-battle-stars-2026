#ifndef sdl_renderer_h
#define sdl_renderer_h

#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Texture/Texture.h"

#include <memory>
#include <SDL.h>
#include <string>

class SDLRenderer : public Renderer
{
public:
    SDLRenderer(SDL_Window* window);
    ~SDLRenderer();

    void clear() override;
    void present() override;

    std::shared_ptr<Texture> createTexture(const std::string& path) override;

    void draw(
        const Texture& texture,
        int x, int y, int width, int height
    ) override;

    void setViewport(int x, int y, int width, int height) override;

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
