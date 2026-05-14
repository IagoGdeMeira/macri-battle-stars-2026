#include "../include/DynamicPushboxResolutionSystem/DynamicPushboxResolutionSystem.h"

#include "../events/CollisionEvent.h"
#include "../include/CollisionHandler/CollisionHandlerFactory.h"
#include "../include/CollisionHandler/ICollisionHandler.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/PushboxComponent.h"
#include "../../domain/components/VelocityComponent.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>

DynamicPushboxResolutionSystem::DynamicPushboxResolutionSystem(EventBus& bus)
{ bus.subscribe<CollisionEvent>([this](const CollisionEvent& e) { this->collisions.push_back(e); }); }

void DynamicPushboxResolutionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& [a, b] : this->collisions)
    {
        if (!comp.has<PushboxComponent>(a) || !comp.has<PushboxComponent>(b)) continue;

        auto& pushA = comp.get<PushboxComponent>(a);
        auto& pushB = comp.get<PushboxComponent>(b);

        if (pushA.type != PushboxComponent::PushboxType::Dynamic ||
            pushB.type != PushboxComponent::PushboxType::Dynamic) continue;

        this->resolveDynamicCollision(ctx, a, b);
    }
    this->collisions.clear();
}

void DynamicPushboxResolutionSystem::resolveDynamicCollision(UpdateContext& ctx, Entity a, Entity b)
{
    auto handlerA = CollisionHandlerFactory::createForEntity(ctx, {a, std::nullopt});
    auto handlerB = CollisionHandlerFactory::createForEntity(ctx, {b, std::nullopt});
    if (!handlerA || !handlerB) return;

    AABB aabbA = handlerA->getAABB(ctx, {a, std::nullopt});
    AABB aabbB = handlerB->getAABB(ctx, {b, std::nullopt});

    float overlapRight = aabbA.right - aabbB.left;
    float overlapLeft = aabbB.right - aabbA.left;
    float overlapBottom = aabbA.bottom - aabbB.top;
    float overlapTop  = aabbB.bottom - aabbA.top;

    float minOverlapX = std::min(overlapRight, overlapLeft);
    float minOverlapY = std::min(overlapBottom, overlapTop);

    TransformComponent& transA = handlerA->getTransform(ctx, {a, std::nullopt});
    TransformComponent& transB = handlerB->getTransform(ctx, {b, std::nullopt});
    auto& comp = ctx.world.components();

    if (minOverlapX < minOverlapY)
    {
        float half = minOverlapX * 0.5f;
        if (overlapRight < overlapLeft) { transA.x -= half; transB.x += half; }
        else { transA.x += half; transB.x -= half; }

        if (comp.has<VelocityComponent>(a)) comp.get<VelocityComponent>(a).vx = 0.0f;
        if (comp.has<VelocityComponent>(b)) comp.get<VelocityComponent>(b).vx = 0.0f;
    }
    else
    {
        float half = minOverlapY * 0.5f;
        if (overlapBottom < overlapTop) { transA.y -= half; transB.y += half; }
        else { transA.y += half; transB.y -= half; }

        if (comp.has<VelocityComponent>(a)) comp.get<VelocityComponent>(a).vy = 0.0f;
        if (comp.has<VelocityComponent>(b)) comp.get<VelocityComponent>(b).vy = 0.0f;
    }
}
