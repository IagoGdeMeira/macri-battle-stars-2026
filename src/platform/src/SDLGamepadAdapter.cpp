#include "SDLGamepadAdapter/SDLGamepadAdapter.h"

#include "SDLKeyTranslator/SDLKeyTranslator.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/events/InputEvent.h"
#include "engine/include/InputSource/InputSource.h"

#include <stdexcept>

SDLGamepadAdapter::SDLGamepadAdapter(EventBus& eventBus, uint32_t assignedPlayer, int joystickIndex) :
    eventBus(eventBus), playerId(assignedPlayer)
{
    this->joystick = SDL_JoystickOpen(joystickIndex);
    if (!this->joystick) throw std::runtime_error("Failed to open joystick");
}

SDLGamepadAdapter::~SDLGamepadAdapter() { if (this->joystick) SDL_JoystickClose(this->joystick); }

void SDLGamepadAdapter::processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events)
{
    for (const auto& e : events)
    {
        if (e->type() != PlatformEvent::Type::Gamepad) continue;
        
        if (auto* btnEvent = dynamic_cast<const GamepadButtonEvent*>(e.get()))
        {
            auto src = InputSource::gamepad(static_cast<GamepadButton>(btnEvent->button));
            this->eventBus.emit<DigitalInputEvent>(src, this->playerId, btnEvent->pressed);
        }
        else if (auto* axisEvent = dynamic_cast<const GamepadAxisEvent*>(e.get())) switch (axisEvent->axis)
        {
            case 0: case 1:
            {
                auto src = InputSource::gamepad(GamepadButton::LeftStick);
                this->eventBus.emit<AnalogInputEvent>(src, this->playerId, axisEvent->value);
                break;
            }
        }
    }
}
