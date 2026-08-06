#include "JumpSystem/JumpSystem.h"
#include "TriggerEvent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/PlayerComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/InputAction/InputAction.h"
#include "domain/value_objects/TriggerId/TriggerId.h"
#include "engine/value_objects/UpdateContext/UpdateContext.h"

void JumpSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    
    auto view = View<InputComponent, VelocityComponent, GroundedComponent, PlayerComponent, JumpComponent>(comp);
    for (auto [entity, input, velocity, grounded, player, jump] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        bool jumpPressed = this->hasInputAction(input, InputAction::Jump);

        if (grounded.onGround && jumpPressed && !jump.isJumping) this->startJump(jump, velocity, entity);

        if (jump.isJumping)
        {
            this->applyJumpForce(jump, velocity, ctx.deltaTime, jumpPressed);
            if (!jumpPressed || jump.timer >= jump.maxTime) this->stopJump(jump);
        }
    }
}

bool JumpSystem::hasInputAction(InputComponent& input, InputAction action) const
{
    auto it = input.actions.find(action);
    return it != input.actions.end() && it->second.pressed;
}

void JumpSystem::startJump(JumpComponent& jump, VelocityComponent& velocity, Entity entity)
{
    jump.isJumping = true;
    jump.timer = 0.f;
    velocity.velocity.y = 0.f;
    this->bus.emit<TriggerEvent>(TriggerEvent{entity, TriggerId::Jump});
}

void JumpSystem::applyJumpForce(JumpComponent& jump, VelocityComponent& velocity, float deltaTime, bool jumpHeld)
{
    if (!jumpHeld || jump.timer >= jump.maxTime) return;

    velocity.velocity.y -= jump.force * deltaTime;
    jump.timer += deltaTime;    
}

void JumpSystem::stopJump(JumpComponent& jump) { jump.isJumping = false; }
