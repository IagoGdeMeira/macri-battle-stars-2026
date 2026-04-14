#include "../include/MovementSystem/MovementSystem.h"

#include "../include/UpdateContext/UpdateContext.h"

#include "../../domain/components/PositionComponent.h"
#include "../../domain/components/VelocityComponent.h"
#include "../../domain/include/ComponentManager/ComponentManager.h"
#include "../../domain/include/View/View.h"

void MovementSystem::update(UpdateContext& context)
{
    auto view = View<PositionComponent, VelocityComponent>(context.world.components());

    for (auto [entity, p, v] : view)
    {
        p.x += v.vx * context.deltaTime;
        p.y += v.vy * context.deltaTime;
    }
}
