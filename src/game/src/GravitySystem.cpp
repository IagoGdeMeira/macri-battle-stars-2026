#include "../include/GravitySystem/GravitySystem.h"

#include "../../domain/components/VelocityComponent.h"
#include "../../domain/components/GravityComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

void GravitySystem::update(UpdateContext& ctx)
{
    auto view = View<VelocityComponent, GravityComponent>(ctx.world.components());   
    for (auto [e, v, g] : view)
    {
        float appliedGravity = this->baseGravity * g.gravityScale;
        v.vy += appliedGravity * ctx.deltaTime;
    }
}
