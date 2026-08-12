#ifndef round_timer_display_system_h
#define round_timer_display_system_h

#include "RoundTimeEvent.h"

#include "engine/include/System/System.h"

#include <vector>

class EventBus;

class RoundTimerDisplaySystem : public System
{
public:
    RoundTimerDisplaySystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    std::vector<RoundTimeEvent> timeEvents;
};

#endif // round_timer_display_system_h
