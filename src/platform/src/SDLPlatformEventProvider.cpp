#include "../include/SDLPlatformEventProvider/SDLPlatformEventProvider.h"

#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include "../../domain/include/Geometry/Geometry.h"

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
            {
                auto& motion = sdlEvent.motion;
                events.push_back(std::make_unique<MouseMotionEvent>(
                    Position{static_cast<float>(motion.xrel), static_cast<float>(motion.yrel)},
                    Position{static_cast<float>(motion.x), static_cast<float>(motion.y)}));
                break;
            }
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            {
                GamepadButton btn = SDLKeyTranslator::toGamepadButton(sdlEvent.jbutton.button);
                bool pressed = (sdlEvent.type == SDL_JOYBUTTONDOWN);
                events.push_back(std::make_unique<GamepadButtonEvent>(static_cast<uint32_t>(btn), pressed));
                break;
            }
            case SDL_JOYAXISMOTION:
            {
                auto& axis = sdlEvent.jaxis;
                events.push_back(std::make_unique<GamepadAxisEvent>(
                    static_cast<uint32_t>(axis.axis), static_cast<float>(axis.value) / 32767.0f));
                break;
            }
            case SDL_WINDOWEVENT:
            {
                auto& win = sdlEvent.window;
                if (win.event != SDL_WINDOWEVENT_RESIZED && win.event != SDL_WINDOWEVENT_SIZE_CHANGED) break;
                events.emplace_back(new WindowResizedPlatformEvent(
                    Dimension2D{ static_cast<float>(win.data1), static_cast<float>(win.data2) }));
                break;
            }
            default: continue;
        }
    }
    return events;
}
