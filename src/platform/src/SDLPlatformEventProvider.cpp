#include "../include/SDLPlatformEventProvider/SDLPlatformEventProvider.h"

#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include "../../engine/include/GamepadButton/GamepadButton.h"
#include "../../engine/include/KeyCode/KeyCode.h"
#include "../../engine/include/MouseButton/MouseButton.h"

#include <SDL.h>

std::vector<std::unique_ptr<PlatformEvent>> SDLPlatformEventProvider::pollEvents()
{
    std::vector<std::unique_ptr<PlatformEvent>> events;
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
            case SDL_QUIT:
                events.push_back(std::make_unique<QuitPlatformEvent>());
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                KeyCode key = SDLKeyTranslator::toKeyCode(sdlEvent.key.keysym.sym);
                bool pressed = (sdlEvent.type == SDL_KEYDOWN);
                events.push_back(std::make_unique<KeyboardEvent>(static_cast<uint32_t>(key), pressed));
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                MouseButton btn = SDLKeyTranslator::toMouseButton(sdlEvent.button.button);
                bool pressed = (sdlEvent.type == SDL_MOUSEBUTTONDOWN);
                events.push_back(std::make_unique<MouseButtonEvent>(static_cast<uint32_t>(btn), pressed));
                break;
            }
            case SDL_MOUSEMOTION:
                events.push_back(std::make_unique<MouseMotionEvent>(
                    static_cast<float>(sdlEvent.motion.xrel),
                    static_cast<float>(sdlEvent.motion.yrel)));
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            {
                GamepadButton btn = SDLKeyTranslator::toGamepadButton(sdlEvent.jbutton.button);
                bool pressed = (sdlEvent.type == SDL_JOYBUTTONDOWN);
                events.push_back(std::make_unique<GamepadButtonEvent>(static_cast<uint32_t>(btn), pressed));
                break;
            }
            case SDL_JOYAXISMOTION:
                events.push_back(std::make_unique<GamepadAxisEvent>(
                    static_cast<uint32_t>(sdlEvent.jaxis.axis),
                    static_cast<float>(sdlEvent.jaxis.value) / 32767.0f));
                break;
            default: continue;
        }
    }
    return events;
}
