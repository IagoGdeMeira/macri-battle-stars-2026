#include "../include/HorizontalMovementSystem/HorizontalMovementSystem.h"

#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/InputAction/InputAction.h"
#include "../../domain/value_objects/StateId/StateId.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

void HorizontalMovementSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<
        InputComponent,
        AnalogInputComponent,
        StateComponent,
        VelocityComponent,
        PlayerComponent>(comp);

    for (auto [entity, input, analog, state, velocity, p_] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        if (!this->canMove(state.current))
        {
            velocity.velocity.x = 0.f;
            continue;
        }

        float targetVx = 0.f;
        if (analog.move.x != 0.f) targetVx = analog.move.x * this->moveSpeed;
        else
        {
            if (this->hasInputAction(input, InputAction::MoveLeft)) targetVx = -this->moveSpeed;
            else if (this->hasInputAction(input, InputAction::MoveRight)) targetVx = this->moveSpeed;
        }

        velocity.velocity.x = targetVx;
    }
}

bool HorizontalMovementSystem::canMove(StateId state) const
{
    if (state.isUnknown()) return false;

    using State = StateId::BaseState;
    switch (static_cast<State>(state.value()))
    {
        case State::Idle:
        case State::Running:
        case State::Walking:
        case State::Jumping:
        case State::Falling:
            return true;
        default:
            return false;
    }
}

bool HorizontalMovementSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}
