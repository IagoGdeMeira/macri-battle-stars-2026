#ifndef sdl_window
#define sdl_window

#include "../../engine/include/Window/Window.h"

#include <SDL.h>

class SDLWindow : public Window
{
public:
    SDLWindow() {}
    ~SDLWindow();

    void create(
        int width,
        int height,
        const char* title
    ) override;

    void setResolution(int width, int height) override;
    void setFullscreen(bool enabled) override;

    void getSize(int& width, int& height) override;

    SDL_Window* getNativeHandle() const { return this->window; }

private:
    SDL_Window* window = nullptr;
};

#endif // sdl_window
