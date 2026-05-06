#ifndef sdl_renderer_h
#define sdl_renderer_h

#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Texture/Texture.h"
#include "../../engine/include/Viewport/Viewport.h"

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

    SDL_Renderer* get() const { return this->renderer; }

    void drawTexture(const Texture& texture, const DrawTextureParams& params) override;
    void drawRectOutline(const Rectangle& rect, const Color& color) override;
    void drawRectFilled(const Rectangle& rect, const Color& color) override;
    void drawCircleOutline(const Circle& circle, const Color& color) override;
    void drawCircleFilled(const Circle& circle, const Color& color) override;

    void setViewport(const Viewport& viewport) override;

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
