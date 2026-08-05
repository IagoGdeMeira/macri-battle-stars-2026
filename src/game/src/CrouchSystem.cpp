#include "CrouchSystem/CrouchSystem.h"

#include "TriggerEvent.h"

#include "domain/components/InputComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"

#include "engine/include/InputMapper/InputMapper.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void CrouchSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, PlayerComponent>(comp);

    for (auto [entity, input, player] : view)
    {
        bool crouchPressed = this->hasInputAction(input, InputAction::Crouch);
        bool entityCrouching = this->wasCrouching[entity];

        if (crouchPressed != entityCrouching)
        {
            if (crouchPressed)
            {
                bool grounded = this->isGrounded(ctx, entity);
                if (grounded) this->bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::Crouched });
            }
            else this->bus.emit<TriggerEvent>(TriggerEvent{ entity, TriggerId::CrouchReleased });
        }
        this->wasCrouching[entity] = crouchPressed;
    }
}

bool CrouchSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}

bool CrouchSystem::isGrounded(UpdateContext& ctx, Entity entity) const
{
    auto& comp = ctx.world.components();
    if (!comp.has<GroundedComponent>(entity)) return false;
    return comp.get<GroundedComponent>(entity).onGround;
}
