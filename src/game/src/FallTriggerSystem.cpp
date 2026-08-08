#include "FallTriggerSystem/FallTriggerSystem.h"

#include "TriggerEvent.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void FallTriggerSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<VelocityComponent, GroundedComponent, PlayerComponent>(comp);

    for (auto [entity, velocity, grounded, player] : view)
    {
        if (grounded.onGround) continue;

        float currentVy = velocity.velocity.y;
        float previousVy = this->previousVelocityY[entity];

        if (previousVy <= 0.f && currentVy > 0.f)
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Fall}); }

        this->previousVelocityY[entity] = currentVy;
    }
}
