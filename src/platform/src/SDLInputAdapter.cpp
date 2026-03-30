#include "../include/SDLInputAdapter/SDLInputAdapter.h"

#include "../../engine/include/EventBus/EventBus.h"

#include "../../engine/events/KeyEvent.h"
#include "../../engine/events/QuitEvent.h"

#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include <SDL.h>

void SDLInputAdapter::poll()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) this->eventBus.emit<QuitEvent>();
        
        if (event.type == SDL_KEYDOWN)
        { this->eventBus.emit<KeyEvent>(SDLKeyTranslator::toKeyCode(event.key.keysym.sym), true); }
        
        if (event.type == SDL_KEYUP)
        { this->eventBus.emit<KeyEvent>(SDLKeyTranslator::toKeyCode(event.key.keysym.sym), false); }
        
        // Handle other events here
    }
}
