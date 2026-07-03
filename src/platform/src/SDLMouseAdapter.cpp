#include "../include/SDLMouseAdapter/SDLMouseAdapter.h"

#include "../../engine/events/InputEvent.h"
#include "../../engine/events/MousePositionEvent.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/InputSource/InputSource.h"

#include <SDL.h>

void SDLMouseAdapter::processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events)
{
    for (const auto& e : events)
    {
        if (e->type() != PlatformEvent::Type::Mouse) continue;
        
        if (const auto* btnEvent = dynamic_cast<const MouseButtonEvent*>(e.get()))
        {
            auto src = InputSource::mouse(static_cast<MouseButton>(btnEvent->button));
            this->eventBus.emit<DigitalInputEvent>(src, this->playerId, btnEvent->pressed);
        }
        else if (const auto* motionEvent = dynamic_cast<const MouseMotionEvent*>(e.get()))
        {
            float normalizedX = motionEvent->delta.x / 100.f;
            if (normalizedX > 1.f) normalizedX = 1.f;
            if (normalizedX < -1.f) normalizedX = -1.f;

            auto src = InputSource::mouse(MouseButton::Left);
            this->eventBus.emit<AnalogInputEvent>(src, this->playerId, normalizedX);
            this->eventBus.emit<MousePositionEvent>(motionEvent->absolute);
        }
        
    }
}
