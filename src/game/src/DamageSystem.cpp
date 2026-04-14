#include "../include/DamageSystem/DamageSystem.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/HealthComponent.h"
#include "../../domain/components/HitboxComponent.h"
#include "../../domain/components/HurtboxComponent.h"
#include "../../domain/include/Entity/Entity.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

DamageSystem::DamageSystem(EventBus& bus) : bus(bus)
{
    this->bus.subscribe<CollisionEvent>([this](const CollisionEvent& event)
    { this->collisions.push_back(event); });
}

void DamageSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    for (const auto& [a, b] : this->collisions)
    {
        Entity attacker = a;
        Entity target = b;

        if (components.has<HitboxComponent>(b) && components.has<HurtboxComponent>(a))
        {
            attacker = b;
            target = a;
        }

        if (!components.has<HitboxComponent>(attacker)) continue;
        if (!components.has<HurtboxComponent>(target)) continue;
        if (!components.has<HealthComponent>(target)) continue;

        const auto& hitbox = components.get<HitboxComponent>(attacker);
        const auto& hurtbox = components.get<HurtboxComponent>(target);

        int damage = static_cast<int>(hitbox.damage * hurtbox.damageMultiplier);

        auto& health = components.get<HealthComponent>(target);
        health.current -= damage;

        if (health.current < 0) health.current = 0;
    }

    this->collisions.clear();
}
