#include "../include/MovementSystem/MovementSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/ComponentManager/ComponentManager.h"
#include "../../domain/include/View/View.h"

void MovementSystem::update(UpdateContext& context)
{
    auto view = View<TransformComponent, VelocityComponent>(context.world.components());

    for (auto [entity, t, v] : view)
    {
        t.x += v.vx * context.deltaTime;
        t.y += v.vy * context.deltaTime;
    }
}
