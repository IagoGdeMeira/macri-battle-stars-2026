#include "../include/SDLGamepadAdapter/SDLGamepadAdapter.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/events/InputEvent.h"
#include "../../engine/include/InputSource/InputSource.h"
#include "../include/SDLKeyTranslator/SDLKeyTranslator.h"

#include <stdexcept>

SDLGamepadAdapter::SDLGamepadAdapter(EventBus& eventBus, uint32_t assignedPlayer, int joystickIndex) :
    eventBus(eventBus), playerId(assignedPlayer)
{
    this->joystick = SDL_JoystickOpen(joystickIndex);
    if (!this->joystick) throw std::runtime_error("Failed to open joystick");
}

SDLGamepadAdapter::~SDLGamepadAdapter() { if (this->joystick) SDL_JoystickClose(this->joystick); }

void SDLGamepadAdapter::poll()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_JOYBUTTONDOWN)
        {
            GamepadButton btn = SDLKeyTranslator::toGamepadButton(event.jbutton.button);
            this->eventBus.emit<DigitalInputEvent>(InputSource::gamepad(btn), this->playerId, true);
        }
        else if (event.type == SDL_JOYBUTTONUP)
        {
            GamepadButton btn = SDLKeyTranslator::toGamepadButton(event.jbutton.button);
            this->eventBus.emit<DigitalInputEvent>(InputSource::gamepad(btn), this->playerId, false);
        }
        else if (event.type == SDL_JOYAXISMOTION)
        {
            float value = static_cast<float>(event.jaxis.value) / 32767.0f;
            if (event.jaxis.axis == 0)
            { 
                this->eventBus.emit<AnalogInputEvent>(InputSource::gamepad(
                    GamepadButton::LeftStick), this->playerId, value);
            }
            else if (event.jaxis.axis == 1)
            {
                this->eventBus.emit<AnalogInputEvent>(InputSource::gamepad(
                    GamepadButton::LeftStick), this->playerId, value);
            }
        }
    }
}