#include "../include/SDLSystemInitializer/SDLSystemInitializer.h"

#include <stdexcept>

SDLSystemInitializer::SDLSystemInitializer(Uint32 flags) : flags(flags) {}

SDLSystemInitializer::~SDLSystemInitializer()
{ if (this->initialized) this->shutdown(); }

void SDLSystemInitializer::initialize()
{
    if (SDL_Init(flags) != 0)
    { throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError()); }

    this->initialized = true;
}

void SDLSystemInitializer::shutdown()
{
    if (this->initialized)
    {
        SDL_Quit();
        this->initialized = false;
    }
}
