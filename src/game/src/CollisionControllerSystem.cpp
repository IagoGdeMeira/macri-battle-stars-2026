#include "CollisionControllerSystem/CollisionControllerSystem.h"

#include "domain/components/HitboxControllerComponent.h"
#include "domain/components/HitboxControllerMapComponent.h"
#include "domain/components/HurtboxControllerComponent.h"
#include "domain/components/HurtboxControllerMapComponent.h"
#include "domain/components/PushboxControllerComponent.h"
#include "domain/components/PushboxControllerMapComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

CollisionControllerSystem::CollisionControllerSystem(EventBus& bus)
{
    bus.subscribe<StateChangedEvent>([this](const StateChangedEvent& e)
    { this->pendingEvents.push_back(e); });
}

void CollisionControllerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& e : this->pendingEvents)
    {
        if (comp.has<HitboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<HitboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) comp.add<HitboxControllerComponent>(e.entity, it->second);
            else comp.remove<HitboxControllerComponent>(e.entity);
        }

        if (comp.has<HurtboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<HurtboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) comp.add<HurtboxControllerComponent>(e.entity, it->second);
            else comp.remove<HurtboxControllerComponent>(e.entity);
        }

        if (comp.has<PushboxControllerMapComponent>(e.entity))
        {
            auto& mapComp = comp.get<PushboxControllerMapComponent>(e.entity);
            auto it = mapComp.map.find(e.current);
            if (it != mapComp.map.end()) comp.add<PushboxControllerComponent>(e.entity, it->second);
            else comp.remove<PushboxControllerComponent>(e.entity);
        }
    }

    this->pendingEvents.clear();
}
