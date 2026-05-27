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

        if (pushA.type == PushboxComponent::PushboxType::Dynamic &&
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

    AABB dynAABB = dynHandler->getAABB(ctx, {dyn, std::nullopt});
    AABB staAABB = staHandler->getAABB(ctx, {sta, std::nullopt});
    AABB overlap
    {
        dynAABB.right - staAABB.left, staAABB.right - dynAABB.left,
        dynAABB.bottom - staAABB.top, staAABB.bottom - dynAABB.top
    };

    Position minOverlap{ std::min(overlap.right, overlap.left), std::min(overlap.bottom, overlap.top) };

    TransformComponent& dynTrans = dynHandler->getTransform(ctx, {dyn, std::nullopt});

    const Position dynCenter{ (dynAABB.left + dynAABB.right) * 0.5f, (dynAABB.top + dynAABB.bottom) * 0.5f };
    const Position staCenter{ (staAABB.left + staAABB.right) * 0.5f, (staAABB.top + staAABB.bottom) * 0.5f };

    auto& comp = ctx.world.components();

    if (minOverlap.x < minOverlap.y)
    {
        float sep = minOverlap.x;
        if (dynCenter.x < staCenter.x) dynTrans.position.x -= sep;
        else dynTrans.position.x += sep;

        if (comp.has<VelocityComponent>(dyn)) comp.get<VelocityComponent>(dyn).velocity.x = 0.f;
    }
    else
    {
        float sep = minOverlap.y;
        if (dynCenter.y < staCenter.y)
        {
            dynTrans.position.y -= sep;
            if (comp.has<VelocityComponent>(dyn))
            {
                auto& vel = comp.get<VelocityComponent>(dyn);
                if (vel.velocity.y > 0.f) vel.velocity.y = 0.f;
            }
        }
        else
        {
            dynTrans.position.y += sep;
            if (comp.has<VelocityComponent>(dyn)) comp.get<VelocityComponent>(dyn).velocity.y = 0.f;
        }
    }
}
