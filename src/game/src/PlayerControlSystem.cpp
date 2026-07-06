#include "../include/PlayerControlSystem/PlayerControlSystem.h"

#include "../events/TriggerEvent.h"

#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/InputAction/InputAction.h"
#include "../../domain/value_objects/StateId/StateId.h"
#include "../../domain/value_objects/TriggerId/TriggerId.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void PlayerControlSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<
        InputComponent,
        AnalogInputComponent,
        StateComponent,
        VelocityComponent,
        PlayerComponent
    >(comp);

    for (auto [entity, input, analog, state, velocity, p_] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen)
            continue;

        bool grounded = comp.has<GroundedComponent>(entity) && comp.get<GroundedComponent>(entity).onGround;

        bool crouchPressed = this->hasInputAction(input, InputAction::Crouch);
        bool wasCrouching = this->wasCrouching[entity];

        if (crouchPressed && !wasCrouching && grounded)
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Crouched}); }
        else if (!crouchPressed && wasCrouching)
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::CrouchReleased}); }

        this->wasCrouching[entity] = crouchPressed;

        bool canMove = this->canMove(state.current);
        if (canMove)
        {
            float targetVx = 0.f;
            if (analog.move.x != 0.f) targetVx = analog.move.x * this->moveSpeed;
            else
            {
                if (this->hasInputAction(input, InputAction::MoveLeft))
                    targetVx = -this->moveSpeed;
                else if (this->hasInputAction(input, InputAction::MoveRight))
                    targetVx = this->moveSpeed;
            }
            velocity.velocity.x = targetVx;
        }
        else velocity.velocity.x = 0.f;
        
        if (grounded && this->hasInputAction(input, InputAction::Jump))
        { velocity.velocity.y = this->jumpImpulse; }

        if (!canMove) continue;

        if (this->hasInputAction(input, InputAction::Punch))
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Punched}); }

        if (this->hasInputAction(input, InputAction::Kick))
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Kicked}); }

        if (this->hasInputAction(input, InputAction::Defend))
        { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Blocked}); }
    }
}

bool PlayerControlSystem::canMove(StateId state) const
{
    if (state.isUnknown()) return false;

    switch (static_cast<StateId::BaseState>(state.value()))
    {
        case StateId::BaseState::Idle:
        case StateId::BaseState::Running:
        case StateId::BaseState::Walking:
        case StateId::BaseState::Jumping:
        case StateId::BaseState::Falling:
            return true;
        default:
            return false;
    }
}

bool PlayerControlSystem::hasInputAction(InputComponent& input, InputAction action) const
{ return input.actions.count(action) && input.actions.at(action).pressed; }

/*
void PlayerControlSystem::applyMovement(UpdateContext& ctx, Entity entity, bool canMove) const
{
    auto& comp = ctx.world.components();
    auto& velocity = comp.get<VelocityComponent>(entity);
    
    if (!canMove) { velocity.velocity.x = 0.f; return; }

    auto& input  = comp.get<InputComponent>(entity);
    auto& analog = comp.get<AnalogInputComponent>(entity);
    float targetVx = 0.f;

    if (analog.move.x != 0.f) targetVx = analog.move.x * this->moveSpeed;
    else
    {
        if (this->hasInputAction(input, InputAction::MoveLeft)) targetVx = -this->moveSpeed;
        else if (this->hasInputAction(input, InputAction::MoveRight)) targetVx = this->moveSpeed;
    }

    velocity.velocity.x = targetVx;
}

void PlayerControlSystem::applyJump(UpdateContext& ctx, Entity entity) const
{
    auto& comp = ctx.world.components();
    auto& velocity = comp.get<VelocityComponent>(entity);
    auto& input = comp.get<InputComponent>(entity);

    bool grounded = false;
    if (comp.has<GroundedComponent>(entity)) grounded = comp.get<GroundedComponent>(entity).onGround;
    if (grounded && this->hasInputAction(input, InputAction::Jump)) velocity.velocity.y = this->jumpImpulse;
}

void PlayerControlSystem::emitTriggers(UpdateContext& ctx, Entity entity, bool canMove)
{
    if (!canMove) return;

    auto& comp  = ctx.world.components();
    auto& input = comp.get<InputComponent>(entity);

    if (this->hasInputAction(input, InputAction::Punch))
    { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Punched}); }

    if (this->hasInputAction(input, InputAction::Kick))
    { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Kicked}); }

    if (this->hasInputAction(input, InputAction::Defend))
    { this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Blocked}); }
}
*/
