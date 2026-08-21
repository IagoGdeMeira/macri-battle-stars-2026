#include "MovementSystem/MovementSystem.h"

#include "domain/components/HitstopComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/ComponentManager/ComponentManager.h"
#include "domain/include/View/View.h"

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
    }
}
