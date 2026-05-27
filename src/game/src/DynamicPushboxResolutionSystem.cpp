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

    AABB boundsA = handlerA->getAABB(ctx, {a, std::nullopt});
    AABB boundsB = handlerB->getAABB(ctx, {b, std::nullopt});
    AABB overlap
    {
        boundsA.right - boundsB.left, boundsB.right - boundsA.left,
        boundsA.bottom - boundsB.top, boundsB.bottom - boundsA.top
    };

    Position minOverlap{ std::min(overlap.right, overlap.left), std::min(overlap.bottom, overlap.top) };
    TransformComponent& transA = handlerA->getTransform(ctx, {a, std::nullopt});
    TransformComponent& transB = handlerB->getTransform(ctx, {b, std::nullopt});
    auto& comp = ctx.world.components();

    const Position centerA{ (boundsA.left + boundsA.right) * 0.5f, (boundsA.top + boundsA.bottom) * 0.5f };
    const Position centerB{ (boundsB.left + boundsB.right) * 0.5f, (boundsB.top + boundsB.bottom) * 0.5f };

    auto& posA = transA.position;
    auto& posB = transB.position;
    if (minOverlap.x < minOverlap.y)
    {
        float half = minOverlap.x * 0.5f;

        if (centerA.x < centerB.x) { posA.x -= half; posB.x += half; }
        else { posA.x += half; posB.x -= half; }

        if (comp.has<VelocityComponent>(a)) comp.get<VelocityComponent>(a).velocity.x = 0.f;
        if (comp.has<VelocityComponent>(b)) comp.get<VelocityComponent>(b).velocity.x = 0.f;
    }
    else
    {
        float half = minOverlap.y * 0.5f;
        if (centerA.y < centerB.y) { posA.y -= half; posB.y += half; }
        else { posA.y += half; posB.y -= half; }

        if (comp.has<VelocityComponent>(a)) comp.get<VelocityComponent>(a).velocity.y = 0.f;
        if (comp.has<VelocityComponent>(b)) comp.get<VelocityComponent>(b).velocity.y = 0.f;
    }
}
