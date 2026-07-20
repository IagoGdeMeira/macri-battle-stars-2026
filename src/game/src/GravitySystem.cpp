#include "GravitySystem/GravitySystem.h"

#include "domain/components/GravityComponent.h"
#include "domain/components/HitstopComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

void GravitySystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<VelocityComponent, GravityComponent>(comp);   

    for (auto [entity, velocity, gravity] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        float appliedGravity = this->baseGravity * gravity.gravityScale;
        velocity.velocity.y += appliedGravity * ctx.deltaTime;
    }
}
