#include "../include/SDLWindow/SDLWindow.h"

#include <stdexcept>

SDLWindow::SDLWindow()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    { throw std::runtime_error("Failed to initialize SDL"); }
}

SDLWindow::~SDLWindow()
{
    if (this->window) SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void SDLWindow::create(
    int width,
    int height,
    const std::string& title
) {
    this->window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );

    if (!this->window) throw std::runtime_error("Failed to create SDL window");
}

void SDLWindow::pollEvents()
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    { if (event.type == SDL_QUIT) this->closeRequested = true; }
}

bool SDLWindow::shouldClose() const { return this->closeRequested; }
