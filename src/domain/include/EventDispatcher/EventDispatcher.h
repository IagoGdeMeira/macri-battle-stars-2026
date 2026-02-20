#ifndef event_dispatcher_h
#define event_dispatcher_h

#include "../EventBus/EventBus.h"

#include <memory>
#include <mutex>
#include <queue>

class EventDispatcher
{
public:
    explicit EventDispatcher(EventBus &bus);
    
    template <typename Event>
    void post(const Event &event);

    void process();

private:
    struct IQueuedEvent
    {
        virtual ~IQueuedEvent() = default;
        virtual void dispatch(EventBus &bus) = 0;
    };

    template <typename Event>
    struct QueuedEvent : IQueuedEvent
    {
        Event event;

        explicit QueuedEvent(const Event &e) : event(e) {}

        void dispatch(EventBus &bus) override { bus.emit<Event>(event); }
    };

    EventBus &bus;

    std::mutex queueMutex;
    std::queue<std::unique_ptr<IQueuedEvent>> queue;
};

#include "EventDispatcher.inl"

#endif // event_dispatcher_h
