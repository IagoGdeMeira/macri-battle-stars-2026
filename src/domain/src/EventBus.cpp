#include "../include/EventBus/EventBus.h"

#include <algorithm>
#include <cstdlib>

EventBus::EventBus() :
    mainThreadId(std::this_thread::get_id()),
    nextId(1) {}

void EventBus::assertMainThread() const
{ if (std::this_thread::get_id() != this->mainThreadId) std::terminate(); }

void EventBus::clear()
{
    this->assertMainThread();
    this->handlers.clear();
}
