#include "../include/StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"

#include "../include/CollisionHandler/CollisionHandlerFactory.h"
#include "../include/CollisionHandler/ICollisionHandler.h"
#include "../events/CollisionEvent.h"

#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/PushboxComponent.h"
#include "../../domain/components/VelocityComponent.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <algorithm>

StaticPushboxResolutionSystem::StaticPushboxResolutionSystem(EventBus& bus)
{ bus.subscribe<CollisionEvent>([this](const CollisionEvent& e) { this->collisions.push_back(e); }); }

void StaticPushboxResolutionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& [a, b] : this->collisions)
    {
        if (!comp.has<PushboxComponent>(a) || !comp.has<PushboxComponent>(b)) continue;

        auto& pushA = comp.get<PushboxComponent>(a);
        auto& pushB = comp.get<PushboxComponent>(b);

        if (
            pushA.type == PushboxComponent::PushboxType::Dynamic &&
            pushB.type == PushboxComponent::PushboxType::Static)
        { this->resolveStaticCollision(ctx, a, b); }
        else if (
            pushB.type == PushboxComponent::PushboxType::Dynamic &&
            pushA.type == PushboxComponent::PushboxType::Static)
        { this->resolveStaticCollision(ctx, b, a); }
    }
    
    this->collisions.clear();
}

void StaticPushboxResolutionSystem::resolveStaticCollision(UpdateContext& ctx, Entity dyn, Entity sta)
{
    auto dynHandler = CollisionHandlerFactory::createForEntity(ctx, {dyn, std::nullopt});
    auto staHandler = CollisionHandlerFactory::createForEntity(ctx, {sta, std::nullopt});
    if (!dynHandler || !staHandler) return;

    AABB dynAabb = dynHandler->getAABB(ctx, {dyn, std::nullopt});
    AABB staAabb = staHandler->getAABB(ctx, {sta, std::nullopt});

    float overlapRight = dynAabb.right - staAabb.left;
    float overlapLeft = staAabb.right - dynAabb.left;
    float overlapBottom = dynAabb.bottom - staAabb.top;
    float overlapTop = staAabb.bottom - dynAabb.top;

    float minOverlapX = std::min(overlapRight, overlapLeft);
    float minOverlapY = std::min(overlapBottom, overlapTop);

    TransformComponent& dynTrans = dynHandler->getTransform(ctx, {dyn, std::nullopt});
    auto& comp = ctx.world.components();

    if (minOverlapX < minOverlapY)
    {
        if (overlapRight < overlapLeft) dynTrans.x -= overlapRight;
        else dynTrans.x += overlapLeft;

        if (comp.has<VelocityComponent>(dyn)) comp.get<VelocityComponent>(dyn).vx = 0.0f;
    }
    else
    {
        if (overlapBottom < overlapTop)
        {
            dynTrans.y -= overlapBottom;
            if (comp.has<VelocityComponent>(dyn))
            {
                auto& vel = comp.get<VelocityComponent>(dyn);
                if (vel.vy > 0.0f) vel.vy = 0.0f;
            }
        }
        else
        {
            dynTrans.y += overlapTop;
            if (comp.has<VelocityComponent>(dyn)) comp.get<VelocityComponent>(dyn).vy = 0.0f;
        }
    }
}
