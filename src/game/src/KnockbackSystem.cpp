#include "KnockbackSystem/KnockbackSystem.h"

#include "DamageEvent.h"

#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/components/KnockbackComponent.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <cmath>

KnockbackSystem::KnockbackSystem(EventBus& bus) : bus(bus)
{
    bus.subscribe<DamageEvent>([this](const DamageEvent& e)
    { this->damageEvents.push_back(e); });
}

void KnockbackSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& e : this->damageEvents)
    {
        if (!comp.has<TransformComponent>(e.target)) continue;
        if (!comp.has<VelocityComponent>(e.target)) continue;

        Position dir = {0.f, 0.f};
        if (comp.has<TransformComponent>(e.attacker))
        {
            const auto& atkTrans = comp.get<TransformComponent>(e.attacker);
            const auto& tgtTrans = comp.get<TransformComponent>(e.target);
            dir.x = tgtTrans.position.x - atkTrans.position.x;
            dir.y = tgtTrans.position.y - atkTrans.position.y;
        }

        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length > 0) { dir.x /= length; dir.y /= length; }

        float baseForce = 300.f;
        float multiplier = 1.f;
        
        if (comp.has<KnockbackComponent>(e.target))
        {
            const auto& knock = comp.get<KnockbackComponent>(e.target);
            multiplier = knock.force * knock.resistance;
        }

        auto& vel = comp.get<VelocityComponent>(e.target);
        vel.velocity.x += dir.x * baseForce * multiplier;
        vel.velocity.y += dir.y * baseForce * multiplier;
    }
    this->damageEvents.clear();
}