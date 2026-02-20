#include <algorithm>

template <typename Event>
EventBus::HandlerId EventBus::subscribe(const std::function<void(const Event &)> &callback)
{
    this->assertMainThread();

    EventBus::HandlerId id = this->nextId++;

    auto wrapper = [callback](const void *ev)
    { callback(*static_cast<const Event *>(ev)); };

    this->handlers[typeid(Event)].emplace_back(id, std::move(wrapper));
    return id;
}

template <typename Event>
void EventBus::unsubscribe(EventBus::HandlerId id)
{
    this->assertMainThread();

    auto it = this->handlers.find(typeid(Event));
    if (it == this->handlers.end()) return;

    auto &vec = it->second;

    vec.erase(
        std::remove_if(
            vec.begin(),
            vec.end(),
            [id](const auto &pair)
            { return pair.first == id; }),
        vec.end());

    if (vec.empty()) this->handlers.erase(it);
}

template <typename Event>
void EventBus::emit(const Event &event)
{
    this->assertMainThread();

    auto it = this->handlers.find(typeid(Event));
    if (it == this->handlers.end()) return;

    auto handlersCopy = it->second;

    for (auto &[id, callback] : handlersCopy) callback(&event);
}
