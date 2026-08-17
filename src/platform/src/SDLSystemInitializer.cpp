#include "SDLSystemInitializer/SDLSystemInitializer.h"

#include <SDL_ttf.h>
#include <stdexcept>

SDLSystemInitializer::SDLSystemInitializer(Uint32 flags) : flags(flags) {}

SDLSystemInitializer::~SDLSystemInitializer() { if (this->initialized) this->shutdown(); }

void SDLSystemInitializer::initialize()
{
    if (SDL_Init(this->flags) != 0) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    if (TTF_Init() != 0) throw std::runtime_error(std::string("TTF_Init failed: ") + TTF_GetError());

    this->initialized = true;
}

void SDLSystemInitializer::shutdown()
{
    if (!this->initialized) return;

    TTF_Quit();
    SDL_Quit();
    this->initialized = false;
}
