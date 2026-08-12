#include "RoundTimerSystem/RoundTimerSystem.h"

#include "RoundTimeEvent.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void RoundTimerSystem::update(UpdateContext& ctx)
{
    this->remaining -= ctx.deltaTime;
    if (this->remaining < 0.f) this->remaining = 0.f;

    int currentSecond = static_cast<int>(this->remaining);
    if (currentSecond != this->lastEmittedSecond)
    {
        this->bus.emit<RoundTimeEvent>(RoundTimeEvent{this->remaining});
        this->lastEmittedSecond = currentSecond;
    }
}
