#include "CollisionUtils/CollisionUtils.h"

#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"

#include <cmath>

void CollisionUtils::updateWorldTransform(World& world, Entity child, Entity parent)
{
    auto& comp = world.components();
    if (!comp.has<LocalTransform>(child)) return;
    if (!comp.has<TransformComponent>(child)) return;
    if (!comp.has<TransformComponent>(parent)) return;

    const auto& local = comp.get<LocalTransform>(child);
    auto& childTransform = comp.get<TransformComponent>(child);
    const auto& parentTransform = comp.get<TransformComponent>(parent);

    float cosR = std::cos(parentTransform.rotation);
    float sinR = std::sin(parentTransform.rotation);

    float rotatedX = local.position.x * cosR - local.position.y * sinR;
    float rotatedY = local.position.x * sinR + local.position.y * cosR;

    childTransform.position.x   = parentTransform.position.x + rotatedX * parentTransform.scale.x;
    childTransform.position.y   = parentTransform.position.y + rotatedY * parentTransform.scale.y;
    childTransform.rotation     = parentTransform.rotation + local.rotation;
    childTransform.scale.x      = parentTransform.scale.x * local.scale.x;
    childTransform.scale.y      = parentTransform.scale.y * local.scale.y;
}
