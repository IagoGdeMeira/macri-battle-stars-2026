#include "../include/SDLPlatformInputFactory/SDLPlatformInputFactory.h"

#include "../include/SDLGamepadAdapter/SDLGamepadAdapter.h"
#include "../include/SDLKeyboardAdapter/SDLKeyboardAdapter.h"
#include "../include/SDLMouseAdapter/SDLMouseAdapter.h"
#include "../include/SDLSystemAdapter/SDLSystemAdapter.h"

#include <SDL.h>

std::unique_ptr<InputAdapter> SDLPlatformInputFactory::createKeyboardAdapter(const InputContext& context)
{ return std::make_unique<SDLKeyboardAdapter>(this->eventBus, context); }

std::unique_ptr<InputAdapter> SDLPlatformInputFactory::createMouseAdapter()
{ return std::make_unique<SDLMouseAdapter>(this->eventBus, 0); }

std::vector<std::unique_ptr<InputAdapter>> SDLPlatformInputFactory::createGamepadAdapters(uint32_t startingPlayerId)
{
    std::vector<std::unique_ptr<InputAdapter>> adapters;

    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i)
    {
        try 
        {
            auto adapter = std::make_unique<SDLGamepadAdapter>(this->eventBus, startingPlayerId + i, i);
            adapters.push_back(std::move(adapter));
        }
        catch (const std::exception&) {}
    }

    return adapters;
}

std::unique_ptr<InputAdapter> SDLPlatformInputFactory::createSystemAdapter()
{ return std::make_unique<SDLSystemAdapter>(this->eventBus); }
