#ifndef sdl_system_initializer_h
#define sdl_system_initializer_h

#include "engine/include/ISystemInitializer/ISystemInitializer.h"

#include <SDL.h>

class SDLSystemInitializer : public ISystemInitializer
{
public:
    explicit SDLSystemInitializer(Uint32 flags = SDL_INIT_VIDEO);
    ~SDLSystemInitializer() override;

    void initialize() override;
    void shutdown() override;

private:
    Uint32 flags;
    bool initialized = false;
};

#endif // sdl_system_initializer_h
