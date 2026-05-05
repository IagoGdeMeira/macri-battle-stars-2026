#include "../include/GravitySystem/GravitySystem.h"

#include "../../domain/components/GravityComponent.h"
#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void GravitySystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GravityComponent>(ctx.world.components());   

    for (auto [entity, v, g] : view)
    {
        if (ctx.world.components().has<HitstopComponent>(entity))
        { if (ctx.world.components().get<HitstopComponent>(entity).frozen) continue; }

        float appliedGravity = this->baseGravity * g.gravityScale;
        v.vy += appliedGravity * ctx.deltaTime;
    }
}
