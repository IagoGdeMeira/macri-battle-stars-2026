#include "MovementSystem/MovementSystem.h"

#include "domain/components/HitstopComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

void MovementSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, VelocityComponent>(comp);

    for (auto [entity, transform, velocity] : view)
    {
        if (comp.has<HitstopComponent>(entity) && comp.get<HitstopComponent>(entity).frozen) continue;

        auto& vel = velocity.velocity;
        auto& pos = transform.position;

        pos.x += vel.x * ctx.deltaTime;
        pos.y += vel.y * ctx.deltaTime;

        if (entity.id == 5 || entity.id == 58) LOG_DEBUG("MovementSystem: entity {} pos.y={:.2f} vel.y={:.2f} dt={:.4f}",
            entity.id, pos.y, vel.y, ctx.deltaTime);
    }
}
