#include "JumpSystem/JumpSystem.h"

#include "TriggerEvent.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"
#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void JumpSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, VelocityComponent, GroundedComponent, PlayerComponent>(comp);

    for (auto [entity, input, velocity, grounded, player] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        bool jumpPressed = this->hasInputAction(input, InputAction::Jump);
        LOG_DEBUG("JumpSystem: player {} entity {} onGround={} jumpPressed={}",
            player.id, entity.id, grounded.onGround, jumpPressed);

        if (!grounded.onGround || !jumpPressed) continue;

        velocity.velocity.y = this->jumpImpulse;
        this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Jump});
    }
}

bool JumpSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}
