#include "RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"

#include "domain/components/ChildrenComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/View/View.h"
#include "domain/include/World/World.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <sstream>

RoundTimerDisplaySystem::RoundTimerDisplaySystem(EventBus &bus) : bus(bus)
{
    this->bus.subscribe<RoundTimeEvent>([this](const RoundTimeEvent& e)
    { this->timeEvents.push_back(e); });
}

void RoundTimerDisplaySystem::update(UpdateContext &ctx)
{
    if (this->timeEvents.empty())return;

    this->processTimeEvents(ctx);
    this->timeEvents.clear();
}

void RoundTimerDisplaySystem::processTimeEvents(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    auto timerView = View<RoundTimerTag>(comp);
    for (auto [timerEntity, tag] : timerView)
    {
        auto textEntityOpt = this->findTextChild(ctx.world, timerEntity);
        if (!textEntityOpt.has_value()) continue;

        this->updateTextEntity(ctx.world, *textEntityOpt, this->timeEvents);
    }
}

std::optional<Entity> RoundTimerDisplaySystem::findTextChild(World& world, Entity timerEntity) const
{
    auto& comp = world.components();

    if (!comp.has<ChildrenComponent>(timerEntity)) return std::nullopt;

    const auto& children = comp.get<ChildrenComponent>(timerEntity).children;
    for (Entity child : children) if (comp.has<UITextComponent>(child)) return child;

    return std::nullopt;
}

void RoundTimerDisplaySystem::updateTextEntity(World& world, Entity textEntity, const std::vector<RoundTimeEvent>& events)
{
    auto& comp = world.components();
    if (!comp.has<UITextComponent>(textEntity)) return;

    auto& text = comp.get<UITextComponent>(textEntity);
    for (const auto& event : events)
    {
        std::ostringstream oss;
        oss << static_cast<int>(event.remainingSeconds);
        text.text = oss.str();
        text.needsUpdate = true;
    }
}
