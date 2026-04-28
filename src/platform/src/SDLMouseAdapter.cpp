#include "../include/SDLMouseAdapter/SDLMouseAdapter.h"

#include "../../engine/events/InputEvent.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/InputSource/InputSource.h"

#include <SDL.h>

SDLMouseAdapter::SDLMouseAdapter(EventBus& eventBus, uint32_t assignedPlayer)
    : eventBus(eventBus), playerId(assignedPlayer)
{
    this->previousButtonState[MouseButton::Left]   = false;
    this->previousButtonState[MouseButton::Right]  = false;
    this->previousButtonState[MouseButton::Middle] = false;
}

void SDLMouseAdapter::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
        {
            bool pressed = (event.type == SDL_MOUSEBUTTONDOWN);
            MouseButton btn;
            switch (event.button.button)
            {
                case SDL_BUTTON_LEFT:   btn = MouseButton::Left;   break;
                case SDL_BUTTON_RIGHT:  btn = MouseButton::Right;  break;
                case SDL_BUTTON_MIDDLE: btn = MouseButton::Middle; break;
                default: continue;
            }
            if (this->previousButtonState[btn] != pressed)
            {
                this->previousButtonState[btn] = pressed;
                this->eventBus.emit<DigitalInputEvent>(
                    InputSource::mouse(btn), this->playerId, pressed);
            }
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            float normalizedX = static_cast<float>(event.motion.xrel) / 100.0f;
            if (normalizedX > 1.0f) normalizedX = 1.0f;
            if (normalizedX < -1.0f) normalizedX = -1.0f;
            this->eventBus.emit<AnalogInputEvent>(
                InputSource::mouse(MouseButton::Left), this->playerId, normalizedX);
        }
    }
}