#include "GravitySystem/GravitySystem.h"

#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/include/View/View.h"

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
        this->applyGravity(velocity, ctx.deltaTime, scale);
    }
}

float GravitySystem::computeGravityScale(const GravityComponent& gravity, const JumpComponent* jump, float velocityY) const
{
    float scale = gravity.gravityScale;
    if (jump)
    {
        if (velocityY < 0) scale *= jump->gravityScaleAsc;
        else scale *= jump->gravityScaleDesc;
    }
    return scale;
}

void GravitySystem::applyGravity(VelocityComponent& velocity, float deltaTime, float scale) const
{
    float appliedGravity = this->baseGravity * scale;
    velocity.velocity.y += appliedGravity * deltaTime;
}
