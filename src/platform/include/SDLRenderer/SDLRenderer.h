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
    ~SDLRenderer() { if (this->renderer) SDL_DestroyRenderer(this->renderer); }

    void clear() override { SDL_RenderClear(this->renderer); }
    void present() override { SDL_RenderPresent(this->renderer); }
    void setViewport(const Viewport& viewport) override;

    SDL_Renderer* get() const { return this->renderer; }

private:
    void drawTextureImpl(const DrawTextureCommand& cmd);
    void drawFontImpl(const DrawFontCommand& cmd);
    void drawRectangleImpl(const DrawRectangleCommand& cmd);
    void drawCircleImpl(const DrawCircleCommand& cmd);

    void drawRectOutline(const Rectangle& rect, const Color& color);
    void drawRectFilled(const Rectangle& rect, const Color& color);
    void drawCircleOutline(const Circle& circle, const Color& color);
    void drawCircleFilled(const Circle& circle, const Color& color);

private:
    SDL_Renderer* renderer = nullptr;
};

#endif // sdl_renderer_h
