#include "../include/SDLSystemAdapter/SDLSystemAdapter.h"

#include "../../engine/events/PlatformEvent.h"
#include "../../engine/events/QuitEvent.h"
#include "../../engine/include/EventBus/EventBus.h"

void SDLSystemAdapter::processEvents(const std::vector<std::unique_ptr<PlatformEvent>>& events)
{
    for (const auto& e : events) if (e->type() == PlatformEvent::Type::Quit)
    { this->eventBus.emit<QuitEvent>(); }
}
