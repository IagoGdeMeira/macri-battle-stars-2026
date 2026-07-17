#include "../include/JumpSystem/JumpSystem.h"

#include "../events/TriggerEvent.h"

#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/InputAction/InputAction.h"
#include "../../domain/value_objects/TriggerId/TriggerId.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

void JumpSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, VelocityComponent, GroundedComponent, PlayerComponent>(comp);

    for (auto [entity, input, velocity, grounded, p_] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        if (!grounded.onGround) continue;

        if (this->hasInputAction(input, InputAction::Jump))
        {
            velocity.velocity.y = this->jumpImpulse;
            this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Jump});
        }
    }
}

bool JumpSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}
