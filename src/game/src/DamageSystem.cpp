#include "DamageSystem/DamageSystem.h"

#include "CollisionEvent.h"
#include "DamageEvent.h"

#include "domain/components/HealthComponent.h"
#include "domain/components/HitboxComponent.h"
#include "domain/components/HurtboxComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/include/EventBus/EventBus.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

DamageSystem::DamageSystem(EventBus &bus) : bus(bus)
{
    this->bus.subscribe<CollisionEvent>([this](const CollisionEvent& event)
    { this->collisions.push_back(event); });
}

void DamageSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& [a, b] : this->collisions)
    {
        Entity attacker = a;
        Entity target = b;

        if (comp.has<HitboxComponent>(b) && comp.has<HurtboxComponent>(a))
        { attacker = b; target = a; }

        if (!comp.has<HitboxComponent>(attacker)) continue;
        if (!comp.has<HurtboxComponent>(target)) continue;
        if (!comp.has<ParentComponent>(target)) continue;

        Entity player = comp.get<ParentComponent>(target).parent;

        if (!comp.has<HealthComponent>(player)) continue;

        const auto& hitbox = comp.get<HitboxComponent>(attacker);
        const auto& hurtbox = comp.get<HurtboxComponent>(target);
        int damage = static_cast<int>(hitbox.damage * hurtbox.damageMultiplier);

        auto& health = comp.get<HealthComponent>(player);
        health.current -= damage;
        if (health.current < 0) health.current = 0;

        uint32_t playerId = 0;
        if (comp.has<PlayerComponent>(player)) playerId = comp.get<PlayerComponent>(player).id;

        this->bus.emit<DamageEvent>(DamageEvent{ attacker, target, playerId, damage, health.current });
    }
    this->collisions.clear();
}
