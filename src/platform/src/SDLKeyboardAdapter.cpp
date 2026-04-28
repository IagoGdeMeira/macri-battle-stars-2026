#include "../include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"

#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/InputContext/InputContext.h"
#include "../../engine/events/InputEvent.h"
#include "../../engine/events/QuitEvent.h"

#include <SDL.h>

SDLKeyboardAdapter::SDLKeyboardAdapter(EventBus& eventBus, const InputContext& inputContext)
    : eventBus(eventBus) { this->buildKeyPlayerMap(inputContext); }

void SDLKeyboardAdapter::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) this->eventBus.emit<QuitEvent>();
        
        else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            KeyCode code = SDLKeyTranslator::toKeyCode(event.key.keysym.sym);
            auto it = this->keyToPlayer.find(code);
            if (it != this->keyToPlayer.end())
            {
                bool pressed = (event.type == SDL_KEYDOWN);
                this->eventBus.emit<DigitalInputEvent>(InputSource::keyboard(code), it->second, pressed);
            }
        }
    }
}

void SDLKeyboardAdapter::buildKeyPlayerMap(const InputContext& context)
{
    for (const auto& [playerId, binding] : context.bindings) 
    { for (const auto& [key, action] : binding.keyMap) this->keyToPlayer[key] = playerId; }
}
