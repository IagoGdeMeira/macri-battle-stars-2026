#include "../collision_detections/CircleCircleCollisionDetection.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

void CircleCircleCollisionDetection::detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& pair : pairs)
    {
        const auto& a = pair.a;
        const auto& b = pair.b;

        if (!comp.has<CircleColliderComponent>(a) || !comp.has<TransformComponent>(a)) continue;
        if (!comp.has<CircleColliderComponent>(b) || !comp.has<TransformComponent>(b)) continue;

        if (this->circleToCircle(a, b, ctx)) ctx.eventBus.emit<CollisionEvent>(CollisionEvent{a, b});   
    }
}

bool CircleCircleCollisionDetection::circleToCircle(Entity a, Entity b, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    const auto& ta = comp.get<TransformComponent>(a);
    const auto& ca = comp.get<CircleColliderComponent>(a);

    const auto& tb = comp.get<TransformComponent>(b);
    const auto& cb = comp.get<CircleColliderComponent>(b);

    auto& posA = ta.position;
    auto& posB = tb.position;
    Position delta { posB.x - posA.x, posB.y - posA.y };
    float distanceSq = delta.x * delta.x + delta.y * delta.y;

    float radiusSum = ca.radius + cb.radius;
    return distanceSq <= radiusSum * radiusSum;
}
