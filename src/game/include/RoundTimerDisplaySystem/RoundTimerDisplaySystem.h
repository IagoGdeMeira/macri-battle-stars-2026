#ifndef round_timer_display_system_h
#define round_timer_display_system_h

#include "RoundTimeEvent.h"

#include "domain/include/Entity/Entity.h"

#include "engine/include/System/System.h"

#include <optional>
#include <vector>

class EventBus;
class World;

class RoundTimerDisplaySystem : public System
{
public:
    RoundTimerDisplaySystem(EventBus& bus);
    void update(UpdateContext& ctx) override;

private:
    void processTimeEvents(UpdateContext& ctx);
    std::optional<Entity> findTextChild(World& world, Entity timerEntity) const;
    void updateTextEntity(World& world, Entity textEntity, const std::vector<RoundTimeEvent>& events);

    EventBus& bus;
    std::vector<RoundTimeEvent> timeEvents;
};

#endif // round_timer_display_system_h
