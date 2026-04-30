#include "../include/PlayerControlSystem/PlayerControlSystem.h"

#include "../events/TriggerEvent.h"
#include "../include/TriggerId/TriggerId.h"

#include "../../domain/components/AnalogInputComponent.h"
#include "../../domain/components/GroundedComponent.h"
#include "../../domain/components/InputComponent.h"
#include "../../domain/components/PlayerComponent.h"
#include "../../domain/components/StateComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/InputAction/InputAction.h"
#include "../../domain/include/StateId/StateId.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

PlayerControlSystem::PlayerControlSystem(EventBus& bus, float moveSpeed, float jumpImpulse) :
    bus(bus), moveSpeed(moveSpeed), jumpImpulse(jumpImpulse) {}

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

    for (auto [entity, input, analog, state, velocity, player] : view)
    {
        bool canMove = false;
        if (state.current == StateId::Idle ||
            state.current == StateId::Walking ||
            state.current == StateId::Running ||
            state.current == StateId::Jumping ||
            state.current == StateId::Falling)
        { canMove = true; }

        if (canMove)
        {
            float targetVx = 0.0f;
            if (analog.moveX != 0.0f) targetVx = analog.moveX * this->moveSpeed;
            else
            {
                if (
                    input.actions.count(InputAction::MoveLeft) &&
                    input.actions.at(InputAction::MoveLeft).pressed
                ) targetVx = -this->moveSpeed;
                else if (
                    input.actions.count(InputAction::MoveRight) &&
                    input.actions.at(InputAction::MoveRight).pressed
                ) targetVx = this->moveSpeed;
            }
            velocity.vx = targetVx;
        }
        else velocity.vx = 0.0f;
        
        bool grounded = false;
        if (comp.has<GroundedComponent>(entity)) grounded = comp.get<GroundedComponent>(entity).onGround;

        if (
            grounded && input.actions.count(InputAction::Jump) &&
            input.actions.at(InputAction::Jump).pressed
        ) velocity.vy = this->jumpImpulse;

        if (
            canMove && input.actions.count(InputAction::Attack) &&
            input.actions.at(InputAction::Attack).pressed
        ) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Punched});

        if (
            canMove && input.actions.count(InputAction::Defend) &&
            input.actions.at(InputAction::Defend).pressed
        ) this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Blocked});
    }
}
