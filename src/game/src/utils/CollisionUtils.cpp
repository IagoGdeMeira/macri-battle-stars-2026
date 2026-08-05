#include "CollisionUtils/CollisionUtils.h"

#include "domain/components/LocalTransform.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/utils/Logger/Logger.h"

#include <cmath>

void CollisionUtils::updateWorldTransform(World& world, Entity child, Entity parent)
{
    auto& comp = world.components();
    if (!comp.has<LocalTransform>(child) || !comp.has<TransformComponent>(child)) return;
    if (!comp.has<TransformComponent>(parent)) return;

    const auto& local = comp.get<LocalTransform>(child);
    auto& childTransform = comp.get<TransformComponent>(child);
    const auto& parentTransform = comp.get<TransformComponent>(parent);

    float orientSign = 1.f;
    if (comp.has<OrientationComponent>(parent))
    {
        Orientation orient = comp.get<OrientationComponent>(parent).direction;
        orientSign = (orient == Orientation::Right) ? 1.f : -1.f;
    }

    float offsetX = local.position.x * orientSign;
    float offsetY = local.position.y;

    float cosR = std::cos(parentTransform.rotation);
    float sinR = std::sin(parentTransform.rotation);

    float rotatedX = offsetX * cosR - offsetY * sinR;
    float rotatedY = offsetX * sinR + offsetY * cosR;

    float newX = parentTransform.position.x + rotatedX * parentTransform.scale.x;
    float newY = parentTransform.position.y + rotatedY * parentTransform.scale.y;

    childTransform.position.x = newX;
    childTransform.position.y = newY;
    childTransform.rotation = parentTransform.rotation + local.rotation;
    childTransform.scale.x = parentTransform.scale.x * local.scale.x;
    childTransform.scale.y = parentTransform.scale.y * local.scale.y;
}
