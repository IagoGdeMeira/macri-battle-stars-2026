#include "StaticPushboxResolutionSystem/StaticPushboxResolutionSystem.h"

#include "CollisionEvent.h"
#include "CollisionHandlerFactory/CollisionHandlerFactory.h"
#include "ICollisionHandler/ICollisionHandler.h"

#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>

StaticPushboxResolutionSystem::StaticPushboxResolutionSystem(EventBus& bus)
{
    bus.subscribe<CollisionEvent>([this](const CollisionEvent& e)
    { this->collisions.push_back(e); });
}

void StaticPushboxResolutionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    for (const auto& [a, b] : this->collisions)
    {
        if (!comp.has<PushboxComponent>(a) || !comp.has<PushboxComponent>(b)) continue;

        auto& pushA = comp.get<PushboxComponent>(a);
        auto& pushB = comp.get<PushboxComponent>(b);

        using Type = PushboxComponent::Type;
        if (pushA.type == Type::Dynamic && pushB.type == Type::Static) this->resolveStaticCollision(ctx, a, b);
        else if (pushB.type == Type::Dynamic && pushA.type == Type::Static) this->resolveStaticCollision(ctx, b, a);
    }
    
    this->collisions.clear();
}

void StaticPushboxResolutionSystem::resolveStaticCollision(UpdateContext& ctx, Entity dyn, Entity sta)
{
    auto& comp = ctx.world.components();

    Entity rootDyn = comp.has<ParentComponent>(dyn) ? comp.get<ParentComponent>(dyn).parent : dyn;
    Entity rootSta = comp.has<ParentComponent>(sta) ? comp.get<ParentComponent>(sta).parent : sta;

    auto dynHandler = CollisionHandlerFactory::createForEntity(ctx, {dyn, std::nullopt});
    auto staHandler = CollisionHandlerFactory::createForEntity(ctx, {sta, std::nullopt});
    if (!dynHandler || !staHandler) return;

    AABB dynAABB = dynHandler->getAABB(ctx, {dyn, std::nullopt});
    AABB staAABB = staHandler->getAABB(ctx, {sta, std::nullopt});
    AABB overlap {
        dynAABB.right - staAABB.left, staAABB.right - dynAABB.left,
        dynAABB.bottom - staAABB.top, staAABB.bottom - dynAABB.top
    };

    Position minOverlap { std::min(overlap.right, overlap.left), std::min(overlap.bottom, overlap.top) };

    TransformComponent& dynTrans = dynHandler->getTransform(ctx, {rootDyn, std::nullopt});

    const Position dynCenter{(dynAABB.left + dynAABB.right) * 0.5f, (dynAABB.top + dynAABB.bottom) * 0.5f};
    const Position staCenter{(staAABB.left + staAABB.right) * 0.5f, (staAABB.top + staAABB.bottom) * 0.5f};

    if (minOverlap.x < minOverlap.y)
    {
        float sep = minOverlap.x;
        if (dynCenter.x < staCenter.x) dynTrans.position.x -= sep;
        else dynTrans.position.x += sep;

        if (comp.has<VelocityComponent>(rootDyn)) comp.get<VelocityComponent>(rootDyn).velocity.x = 0.f;
    }
    else
    {
        float sep = minOverlap.y;
        if (dynCenter.y < staCenter.y)
        {
            dynTrans.position.y -= sep;
            if (!comp.has<VelocityComponent>(rootDyn)) return;
            
            auto& vel = comp.get<VelocityComponent>(rootDyn);
            if (vel.velocity.y > 0.f) vel.velocity.y = 0.f;
        }
        else
        {
            dynTrans.position.y += sep;
            if (comp.has<VelocityComponent>(rootDyn)) comp.get<VelocityComponent>(rootDyn).velocity.y = 0.f;
        }
    }
}
