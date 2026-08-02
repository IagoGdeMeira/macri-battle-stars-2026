#include "CollisionUtils/CollisionUtils.h"

#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/utils/Logger/Logger.h"

#include <cmath>

void CollisionUtils::updateWorldTransform(World& world, Entity child, Entity parent)
{
    auto &comp = world.components();
    if (!comp.has<LocalTransform>(child) || !comp.has<TransformComponent>(child) ||
        !comp.has<TransformComponent>(parent))
        return;

    const auto &local = comp.get<LocalTransform>(child);
    auto &childTransform = comp.get<TransformComponent>(child);
    const auto &parentTransform = comp.get<TransformComponent>(parent);

    float cosR = std::cos(parentTransform.rotation);
    float sinR = std::sin(parentTransform.rotation);

    float rotatedX = local.position.x * cosR - local.position.y * sinR;
    float rotatedY = local.position.x * sinR + local.position.y * cosR;

    float newX = parentTransform.position.x + rotatedX * parentTransform.scale.x;
    float newY = parentTransform.position.y + rotatedY * parentTransform.scale.y;

    LOG_DEBUG("CollisionUtils: child {} before=({},{}) local=({},{}) parent=({},{}) after=({},{})", 
        child.id, childTransform.position.x, childTransform.position.y,
        local.position.x, local.position.y, parentTransform.position.x, parentTransform.position.y,
        newX, newY);

    childTransform.position.x   = newX;
    childTransform.position.y   = newY;
    childTransform.rotation     = parentTransform.rotation + local.rotation;
    childTransform.scale.x      = parentTransform.scale.x * local.scale.x;
    childTransform.scale.y      = parentTransform.scale.y * local.scale.y;
}
