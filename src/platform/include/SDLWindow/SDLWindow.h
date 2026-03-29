#ifndef sdl_window
#define sdl_window

#include "../../engine/include/Window/Window.h"

#include <SDL.h>

class SDLWindow : public Window
{
public:
    SDLWindow();
    ~SDLWindow();

    void create(int width, int height, const std::string& title) override;
    void pollEvents() override;
    bool shouldClose() const override;

private:
    SDL_Window* window = nullptr;
    bool closeRequested = false;
};

#endif // sdl_window
