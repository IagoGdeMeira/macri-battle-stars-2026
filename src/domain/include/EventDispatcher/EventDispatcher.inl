template <typename Event>
void EventDispatcher::post(const Event &event)
{
    std::lock_guard<std::mutex> lock(this->queueMutex);
    this->queue.push(std::make_unique<QueuedEvent<Event>>(event));
}
