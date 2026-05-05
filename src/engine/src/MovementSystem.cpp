#include "../include/MovementSystem/MovementSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/components/HitstopComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/ComponentManager/ComponentManager.h"
#include "../../domain/include/View/View.h"

void MovementSystem::update(UpdateContext& ctx)
{
    auto view = View<TransformComponent, VelocityComponent>(ctx.world.components());

    for (auto [entity, t, v] : view)
    {
        if (ctx.world.components().has<HitstopComponent>(entity))
        { if (ctx.world.components().get<HitstopComponent>(entity).frozen) continue; }

        t.x += v.vx * ctx.deltaTime;
        t.y += v.vy * ctx.deltaTime;
    }
}
