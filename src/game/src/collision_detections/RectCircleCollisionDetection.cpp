#include "../collision_detections/RectCircleCollisionDetection.h"

#include "../events/CollisionEvent.h"

#include "../../domain/components/CircleColliderComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/Geometry/Geometry.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>
#include <cmath>

void RectCircleCollisionDetection::detect(const std::vector<CollisionPair>& pairs, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& pair : pairs)
    {
        const auto& a = pair.a;
        const auto& b = pair.b;

        bool aRect = comp.has<RectangleColliderComponent>(a) && comp.has<TransformComponent>(a);
        bool bRect = comp.has<RectangleColliderComponent>(b) && comp.has<TransformComponent>(b);
        bool aCircle = comp.has<CircleColliderComponent>(a) && comp.has<TransformComponent>(a);
        bool bCircle = comp.has<CircleColliderComponent>(b) && comp.has<TransformComponent>(b);

        if (aRect && bCircle) if (this->rectToCircle(a, b, ctx)) ctx.eventBus.emit<CollisionEvent>(CollisionEvent{a, b});
        else if (aCircle && bRect) if (this->rectToCircle(b, a, ctx)) ctx.eventBus.emit<CollisionEvent>(CollisionEvent{a, b});
    }
}

bool RectCircleCollisionDetection::rectToCircle(Entity rect, Entity circle, UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    const auto& trect = comp.get<TransformComponent>(rect);
    const auto& rectCollider = comp.get<RectangleColliderComponent>(rect);

    const auto& tcircle = comp.get<TransformComponent>(circle);
    const auto& circleCollider = comp.get<CircleColliderComponent>(circle);

    auto& size = rectCollider.size;

    auto& posA = trect.position;
    AABB rectBounds
    {
        posA.x - size.width * 0.5f, posA.x + size.width * 0.5f,
        posA.y - size.height * 0.5f, posA.y + size.height * 0.5f
    };

    Position closest =
    {
        std::max(rectBounds.left, std::min(tcircle.position.x, rectBounds.right)),
        std::max(rectBounds.top, std::min(tcircle.position.y, rectBounds.bottom))
    };

    auto& posB = tcircle.position;
    Position delta { posB.x - closest.x, posB.y - closest.y };
    float distanceSq = delta.x * delta.x + delta.y * delta.y;

    return distanceSq <= circleCollider.radius * circleCollider.radius;
}
