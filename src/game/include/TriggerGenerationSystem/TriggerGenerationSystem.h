#ifndef trigger_generation_system_h
#define trigger_generation_system_h

#include "../TriggerContext/TriggerContext.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/System/System.h"

class TriggerGenerationSystem : public System
{
public:
    TriggerGenerationSystem(EventBus& bus, TriggerContext& context) :
        bus(bus), context(context) {}

    void update(UpdateContext& ctx) override;

private:
    EventBus& bus;
    TriggerContext& context;

    void processInputTriggers(UpdateContext& ctx);
};

#endif // trigger_generation_system_h
