#include "GroundDetectionSystem/GroundDetectionSystem.h"

#include "CollisionEvent.h"
#include "CollisionHandlerFactory/CollisionHandlerFactory.h"
#include "ICollisionHandler/ICollisionHandler.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/GroundedComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/PushboxComponent.h"
#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VelocityComponent.h"
#include "domain/include/Entity/Entity.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <algorithm>

GroundDetectionSystem::GroundDetectionSystem(EventBus& bus)
{
    bus.subscribe<CollisionEvent>([this](const CollisionEvent& e)
    { this->collisions.push_back(e); });
}

void GroundDetectionSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();

    auto groundedView = View<GroundedComponent>(comp);
    for (auto [entity, grounded] : groundedView) grounded.onGround = false;
    for (const auto& [a, b] : collisions)
    {
        if (comp.has<ActiveComponent>(a) && !comp.get<ActiveComponent>(a).active) continue;
        if (comp.has<ActiveComponent>(b) && !comp.get<ActiveComponent>(b).active) continue;

        if (this->isStaticPushbox(ctx, a)) this->processGroundCollision(ctx, a, b);
        else if (this->isStaticPushbox(ctx, b)) this->processGroundCollision(ctx, b, a);
    }

    this->collisions.clear();
}

bool GroundDetectionSystem::isStaticPushbox(UpdateContext& ctx, Entity entity) const
{
    auto& comp = ctx.world.components();
    if (!comp.has<PushboxComponent>(entity)) return false;
    return comp.get<PushboxComponent>(entity).type == PushboxComponent::Type::Static;
}

void GroundDetectionSystem::processGroundCollision(UpdateContext& ctx, Entity staticEntity, Entity dynamicCollider)
{
    std::optional<Entity> owner = this->getGroundedOwner(ctx, dynamicCollider);
    if (!owner.has_value()) return;

    auto& comp = ctx.world.components();
    if (comp.has<VelocityComponent>(*owner))
    {
        const auto& vel = comp.get<VelocityComponent>(*owner);
        if (vel.velocity.y < 0) return;
    }

    if (!this->isStandingOnGround(ctx, dynamicCollider, staticEntity, *owner)) return;

    comp.get<GroundedComponent>(*owner).onGround = true;
}

bool GroundDetectionSystem::isStandingOnGround(UpdateContext& ctx, Entity dynamicCollider, Entity staticEntity, Entity owner) const
{
    auto dynHandler = CollisionHandlerFactory::createForEntity(ctx, {dynamicCollider, std::nullopt});
    auto staHandler = CollisionHandlerFactory::createForEntity(ctx, {staticEntity, std::nullopt});
    if (!dynHandler || !staHandler) return false;

    AABB dynAabb = dynHandler->getAABB(ctx, {dynamicCollider, owner});
    AABB staAabb = staHandler->getAABB(ctx, {staticEntity, std::nullopt});

    float overlapRight = dynAabb.right - staAabb.left;
    float overlapLeft = staAabb.right - dynAabb.left;
    float overlapBottom = dynAabb.bottom - staAabb.top;
    float overlapTop = staAabb.bottom - dynAabb.top;

    float minOverlapX = std::min(overlapRight, overlapLeft);
    float minOverlapY = std::min(overlapBottom, overlapTop);

    return minOverlapY < minOverlapX && overlapBottom < overlapTop;
}

std::optional<Entity> GroundDetectionSystem::getGroundedOwner(UpdateContext& ctx, Entity colliderEntity) const
{
    auto& comp = ctx.world.components();
    if (comp.has<GroundedComponent>(colliderEntity)) return colliderEntity;

    Entity current = colliderEntity;
    while (comp.has<ParentComponent>(current))
    {
        Entity parent = comp.get<ParentComponent>(current).parent;
        if (comp.has<GroundedComponent>(parent)) return parent;
        current = parent;
    }
    return std::nullopt;
}
