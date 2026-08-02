#include "CollisionHandlerFactory/CollisionHandlerFactory.h"

#include "CircleCollisionHandler.h"
#include "RectangleCollisionHandler.h"

#include "domain/components/RectangleColliderComponent.h"
#include "domain/components/CircleColliderComponent.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

std::unique_ptr<ICollisionHandler> CollisionHandlerFactory::createForEntity(UpdateContext& ctx, ICollisionHandler::EntityParams params)
{
    auto& comp = ctx.world.components();

    using RectColComp = RectangleColliderComponent;
    using CircColComp = CircleColliderComponent;

    if (comp.has<RectColComp>(params.preferred) || (params.fallback.has_value() && comp.has<RectColComp>(*params.fallback)))
    { return std::make_unique<RectangleCollisionHandler>(); }

    if (comp.has<CircColComp>(params.preferred) || (params.fallback.has_value() && comp.has<CircColComp>(*params.fallback)))
    { return std::make_unique<CircleCollisionHandler>(); }

    return nullptr;
}
