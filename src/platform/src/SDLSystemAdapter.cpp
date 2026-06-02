#include "../include/SDLSystemAdapter/SDLSystemAdapter.h"

#include "../../engine/events/PlatformEvent.h"
#include "../../engine/events/QuitEvent.h"
#include "../../engine/events/WindowResizedEvent.h"
#include "../../engine/include/EventBus/EventBus.h"

void SDLSystemAdapter::processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events)
{
    auto& bus = this->eventBus;
    for (const auto& e : events)
    {
        switch(e->type())
        {
            case PlatformEvent::Type::Quit: bus.emit<QuitEvent>(); break;
            case PlatformEvent::Type::WindowResized:
            {
                auto* resizeEvent = dynamic_cast<const WindowResizedPlatformEvent*>(e.get());
                if (resizeEvent) bus.emit<WindowResizedEvent>(resizeEvent->size);
                break;
            }
            default: break;
        }
    }
}
