#include "../include/SDLWindow/SDLWindow.h"

#include "../../engine/events/QuitEvent.h"
#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/EventBus/EventBus.h"

#include <stdexcept>

SDLWindow::~SDLWindow()
{
    if (this->window) SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void SDLWindow::create(
    int width,
    int height,
    const char* title
) {
    SDL_Init(SDL_INIT_VIDEO);

    this->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!this->window) throw std::runtime_error("Failed to create SDL window");
}

void SDLWindow::setResolution(int width, int height)
{ if (this->window) SDL_SetWindowSize(this->window, width, height); }

void SDLWindow::setFullscreen(bool enabled)
{
    if (!this->window) return;
    SDL_SetWindowFullscreen(this->window, enabled ? SDL_WINDOW_FULLSCREEN : 0);
}

void SDLWindow::getSize(int& width, int& height)
{
    if (!this->window) return;
    SDL_GetWindowSize(this->window, &width, &height);
}
