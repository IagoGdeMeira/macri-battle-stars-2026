#include "RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"

#include "domain/components/RoundTimerTag.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/View/View.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <sstream>

RoundTimerDisplaySystem::RoundTimerDisplaySystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<RoundTimeEvent>([this](const RoundTimeEvent& e)
    { this->timeEvents.push_back(e); });
}

void RoundTimerDisplaySystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<RoundTimerTag, UITextComponent>(comp);

    for (const auto& event : this->timeEvents) for (auto [entity, tag, text] : view)
    {
        std::ostringstream oss;
        oss << static_cast<int>(event.remainingSeconds);
        text.text = oss.str();
        text.needsUpdate = true;
    }
    
    this->timeEvents.clear();
}
