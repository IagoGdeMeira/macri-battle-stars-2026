#include "../include/CollisionHandler/CollisionHandlerFactory.h"

#include "../include/CollisionHandler/RectangleCollisionHandler.h"
#include "../include/CollisionHandler/CircleCollisionHandler.h"

#include "../../domain/components/RectangleColliderComponent.h"
#include "../../domain/components/CircleColliderComponent.h"
#include "../../engine/value_objects/UpdateContext/UpdateContext.h"

std::unique_ptr<ICollisionHandler> CollisionHandlerFactory::createForEntity(
    UpdateContext& ctx,
    ICollisionHandler::EntityParams params
) {
    auto& components = ctx.world.components();

    if (components.has<RectangleColliderComponent>(params.preferred) ||
        (params.fallback.has_value() && components.has<RectangleColliderComponent>(*params.fallback)))
    { return std::make_unique<RectangleCollisionHandler>(); }

    if (components.has<CircleColliderComponent>(params.preferred) ||
        (params.fallback.has_value() && components.has<CircleColliderComponent>(*params.fallback)))
    { return std::make_unique<CircleCollisionHandler>(); }

    return nullptr;
}
