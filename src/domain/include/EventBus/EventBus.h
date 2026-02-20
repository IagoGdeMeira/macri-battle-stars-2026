#ifndef event_bus_h
#define event_bus_h

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <thread>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventDispatcher;

class EventBus
{
public:
    using HandlerId = uint64_t;

    EventBus();

    template <typename Event>
    HandlerId subscribe(const std::function<void(const Event &)> &callback);

    template <typename Event>
    void unsubscribe(HandlerId id);

    void clear();

private:
    friend class EventDispatcher;

    template <typename Event>
    void emit(const Event &event);

    void assertMainThread() const;

private:
    using Handler = std::function<void(const void *)>;
    using HandlerList = std::vector<std::pair<HandlerId, Handler>>;

    std::unordered_map<std::type_index, HandlerList> handlers;

    std::thread::id mainThreadId;
    uint64_t nextId;
};

#include "EventBus.inl"

#endif // event_bus_h
