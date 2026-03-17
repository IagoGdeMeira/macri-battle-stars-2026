#include "EventBus.h"

#include <utility>

template <typename Event>
void EventBus::subscribe(Handler<Event> handler)
{
    auto& list = handlers[typeid(Event)];

    list.push_back([handler](const void* eventPtr)
        { handler(*static_cast<const Event*>(eventPtr)); });
}

template <typename Event, typename... Args>
void EventBus::emit(Args &&...args)
{
    auto it = handlers.find(typeid(Event));

    if (it == handlers.end()) return;

    Event event(std::forward<Args>(args)...);

    for (auto& handler : it->second) handler(&event);
}