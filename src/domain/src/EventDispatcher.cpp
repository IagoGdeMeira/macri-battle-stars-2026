#include "../include/EventDispatcher/EventDispatcher.h"

EventDispatcher::EventDispatcher(EventBus &bus) : bus(bus) {}

void EventDispatcher::process()
{
    std::queue<std::unique_ptr<IQueuedEvent>> localQueue;

    {
        std::lock_guard<std::mutex> lock(this->queueMutex);
        std::swap(localQueue, this->queue);
    }

    while (!localQueue.empty())
    {
        localQueue.front()->dispatch(this->bus);
        localQueue.pop();
    }
}
