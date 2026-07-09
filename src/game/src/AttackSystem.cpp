#include "../include/AttackSystem/AttackSystem.h"

#include "../events/TriggerEvent.h"

#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/StateId/StateId.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void AttackSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<InputComponent, PlayerComponent>(comp);

    for (auto [entity, input, p_] : view)
    {
        if (!this->canAttack(ctx, entity)) continue;

        if (this->hasInputAction(input, this->action))
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, this->trigger}); }
    }
}

bool AttackSystem::canAttack(UpdateContext& ctx, Entity entity) const
{
    auto& comp = ctx.world.components();

    if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) return false;

    if (comp.has<StateComponent>(entity))
    {
        StateId current = comp.get<StateComponent>(entity).current;
        
        switch (current.value())
        {
            case static_cast<std::int32_t>(StateId::BaseState::Idle):
            case static_cast<std::int32_t>(StateId::BaseState::Running):
            case static_cast<std::int32_t>(StateId::BaseState::Walking):
            case static_cast<std::int32_t>(StateId::BaseState::Crouching):
                return true;
            default:
                return false;
        }
    }

    return true;
}

bool AttackSystem::hasInputAction(InputComponent& input, InputAction inputAction) const
{
    auto it = input.actions.find(inputAction);
    return it != input.actions.end() && it->second.pressed;
}
