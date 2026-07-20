#include "HitstopSystem/HitstopSystem.h"

#include "domain/components/HitstopComponent.h"
#include "domain/events/DamageEvent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

HitstopSystem::HitstopSystem(EventBus& bus, float duration) : bus(bus), hitstopDuration(duration)
{
    bus.subscribe<DamageEvent>([this](const DamageEvent& e)
    { this->damageEvents.push_back(e); });
}

void HitstopSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& e : this->damageEvents) for (Entity entity : {e.attacker, e.target})
    {
        if (!comp.has<HitstopComponent>(entity)) continue;
        auto& hitstop = comp.get<HitstopComponent>(entity);
        
        hitstop.remaining = this->hitstopDuration;
        hitstop.frozen = true;
    }
    
    this->damageEvents.clear();

    auto view = View<HitstopComponent>(comp);
    for (auto [entity, hitstop] : view)
    {
        if (!hitstop.frozen) continue;
        hitstop.remaining -= ctx.deltaTime;
        
        if (hitstop.remaining > 0.f) continue;
        hitstop.frozen = false;
        hitstop.remaining = 0.f;
    }
}
