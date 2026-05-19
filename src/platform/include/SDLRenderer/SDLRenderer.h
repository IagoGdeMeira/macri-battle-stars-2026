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

    void drawTexture(const DrawTextureCommand& cmd) override;
    void drawFont(const DrawFontCommand& cmd) override;
    void drawRectangle(const DrawRectangleCommand& cmd) override;
    void drawCircle(const DrawCircleCommand& cmd) override;

    void setViewport(const Viewport& viewport) override;

private:
    void drawRectOutline(const Rectangle& rect, const Color& color);
    void drawRectFilled(const Rectangle& rect, const Color& color);
    void drawCircleOutline(const Circle& circle, const Color& color);
    void drawCircleFilled(const Circle& circle, const Color& color);

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
