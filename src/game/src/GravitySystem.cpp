#include "../include/GravitySystem/GravitySystem.h"

#include "../../domain/components/GravityComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

void GravitySystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GravityComponent>(ctx.world.components());   

    for (auto [entity, v, g] : view)
    {
        auto& comp = ctx.world.components();
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        float appliedGravity = this->baseGravity * g.gravityScale;
        v.velocity.y += appliedGravity * ctx.deltaTime;
    }
}
