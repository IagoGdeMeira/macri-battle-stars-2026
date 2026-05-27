#include "../include/KnockbackSystem/KnockbackSystem.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/components/KnockbackComponent.h"
#include "../../domain/events/DamageEvent.h"
#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

KnockbackSystem::KnockbackSystem(EventBus& bus) : bus(bus)
{
    bus.subscribe<DamageEvent>([this](const DamageEvent& e)
    { this->damageEvents.push_back(e); });
}

void KnockbackSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    for (const auto& e : this->damageEvents)
    {
        if (!components.has<TransformComponent>(e.target)) continue;
        if (!components.has<VelocityComponent>(e.target)) continue;

        Position dir = {0.0f, 0.0f};
        if (components.has<TransformComponent>(e.attacker))
        {
            const auto& atkTrans = components.get<TransformComponent>(e.attacker);
            const auto& tgtTrans = components.get<TransformComponent>(e.target);
            dir.x = tgtTrans.position.x - atkTrans.position.x;
            dir.y = tgtTrans.position.y - atkTrans.position.y;
        }

        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (length > 0) { dir.x /= length; dir.y /= length; }

        float baseForce = 300.0f;
        float multiplier = 1.0f;
        
        if (components.has<KnockbackComponent>(e.target))
        {
            const auto& knock = components.get<KnockbackComponent>(e.target);
            multiplier = knock.force * knock.resistance;
        }

        auto& vel = components.get<VelocityComponent>(e.target);
        vel.velocity.x += dir.x * baseForce * multiplier;
        vel.velocity.y += dir.y * baseForce * multiplier;
    }
    this->damageEvents.clear();
}