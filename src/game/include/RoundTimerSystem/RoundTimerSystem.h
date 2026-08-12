#ifndef round_timer_system_h
#define round_timer_system_h

#include "engine/include/System/System.h"

class EventBus;

class RoundTimerSystem : public System
{
public:
    RoundTimerSystem(EventBus& bus, float initialTime = 99.f) :
        bus(bus), remaining(initialTime), lastEmittedSecond(static_cast<int>(initialTime)) {}
    
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    float remaining;
    int lastEmittedSecond;
};

#endif // round_timer_system_h
