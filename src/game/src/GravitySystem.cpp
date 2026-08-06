#include "GravitySystem/GravitySystem.h"

#include "domain/components/GravityComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
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

        float appliedGravity = this->baseGravity * gravity.gravityScale;
        velocity.velocity.y += appliedGravity * ctx.deltaTime;
    }
}
