#include "../collision_detections/RectRectCollisionDetection.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

void RectRectCollisionDetection::detect(const std::vector<ICollisionDetection::CollisionPair>& pairs, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& pair : pairs)
    {
        const auto& a = pair.a;
        const auto& b = pair.b;

        if (!comp.has<RectangleColliderComponent>(a) || !comp.has<TransformComponent>(a)) continue;
        if (!comp.has<RectangleColliderComponent>(b) || !comp.has<TransformComponent>(b)) continue;

        if (this->rectToRect(a, b, ctx)) ctx.eventBus.emit<CollisionEvent>(CollisionEvent{a, b});
    }
}

bool RectRectCollisionDetection::rectToRect(Entity a, Entity b, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    const auto& ta = comp.get<TransformComponent>(a);
    const auto& ra = comp.get<RectangleColliderComponent>(a);

    const auto& tb = comp.get<TransformComponent>(b);
    const auto& rb = comp.get<RectangleColliderComponent>(b);

    auto& posA = ta.position;
    const AABB rectABounds
    {
        posA.x - ra.size.width * 0.5f, posA.x + ra.size.width * 0.5f,
        posA.y - ra.size.height * 0.5f, posA.y + ra.size.height * 0.5f
    };

    auto& posB = tb.position;
    const AABB rectBBounds
    {
        posB.x - rb.size.width * 0.5f, posB.x + rb.size.width * 0.5f,
        posB.y - rb.size.height * 0.5f, posB.y + rb.size.height * 0.5f
    };

    if (rectABounds.left >= rectBBounds.right || rectBBounds.left >= rectABounds.right) return false;
    if (rectABounds.top >= rectBBounds.bottom || rectBBounds.top >= rectABounds.bottom) return false;
    return true;
}
