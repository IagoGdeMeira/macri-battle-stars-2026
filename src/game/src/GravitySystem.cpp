#include "GravitySystem/GravitySystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/InputComponent.h"
#include "domain/components/JumpComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/InputAction/InputAction.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void GravitySystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<VelocityComponent, GravityComponent, GroundedComponent>(comp);

    for (auto [entity, velocity, gravity, grounded] : view)
    {
        if (grounded.onGround) continue;
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        const JumpComponent* jump = comp.has<JumpComponent>(entity) ? &comp.get<JumpComponent>(entity) : nullptr;
        float scale = this->computeGravityScale(gravity, jump, velocity.velocity.y);

        if (jump && velocity.velocity.y > 0.f && comp.has<InputComponent>(entity))
        {
            const auto& input = comp.get<InputComponent>(entity);
            auto it = input.actions.find(InputAction::Crouch);
            if (it != input.actions.end() && it->second.pressed) scale *= jump->fastFallMultiplier;
        }

        this->applyGravity(velocity, ctx.deltaTime, scale);
    }
}

float GravitySystem::computeGravityScale(const GravityComponent& gravity, const JumpComponent* jump, float velocityY) const
{
    float scale = gravity.gravityScale;
    if (jump)
    {
        if (velocityY < 0) scale *= jump->gravityScaleAsc;
        else if (velocityY > 0) scale *= jump->gravityScaleDesc; 
    }
    return scale;
}

void GravitySystem::applyGravity(VelocityComponent& velocity, float deltaTime, float scale) const
{
    float appliedGravity = this->baseGravity * scale;
    velocity.velocity.y += appliedGravity * deltaTime;
}
