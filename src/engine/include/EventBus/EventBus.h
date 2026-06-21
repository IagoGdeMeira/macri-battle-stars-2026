#ifndef event_bus_h
#define event_bus_h

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventBus
{
public:
    template <typename Event>
    using Handler = std::function<void(const Event&)>;

    template <typename Event>
    void subscribe(Handler<Event> handler);

    template <typename Event, typename... Args>
    void emit(Args&&...args);

private:
    using HandlerBase = std::function<void(const void*)>;
    std::unordered_map<std::type_index, std::vector<HandlerBase>> handlers;
};

#include "EventBus.inl"

#endif // event_bus_h
