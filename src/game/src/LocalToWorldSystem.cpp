#include "../include/LocalToWorldSystem/LocalToWorldSystem.h"

#include "../../domain/components/LocalTransform.h"
#include "../../domain/components/ParentComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/UpdateContext/UpdateContext.h"

#include <cmath>

void LocalToWorldSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    auto view = View<TransformComponent, LocalTransform, ParentComponent>(components);

    for (auto [entity, transform, local, parent] : view)
    {
        if (!components.has<TransformComponent>(parent.parent)) continue;

        const auto& parentTransform = components.get<TransformComponent>(parent.parent);
        this->applyParentTransform(transform, local, parentTransform);
    }
}

float LocalToWorldSystem::rotateLocalX(const LocalTransform& local, float cosR, float sinR)
{ return local.x * cosR - local.y * sinR; }

float LocalToWorldSystem::rotateLocalY(const LocalTransform& local, float cosR, float sinR)
{ return local.x * sinR + local.y * cosR; }

void LocalToWorldSystem::applyParentTransform(
    TransformComponent& transform,
    const LocalTransform& local,
    const TransformComponent& parentTransform
) {
    float cosR = std::cos(parentTransform.rotation);
    float sinR = std::sin(parentTransform.rotation);

    float rotatedX = LocalToWorldSystem::rotateLocalX(local, cosR, sinR);
    float rotatedY = LocalToWorldSystem::rotateLocalY(local, cosR, sinR);

    transform.x = parentTransform.x + rotatedX * parentTransform.scaleX;
    transform.y = parentTransform.y + rotatedY * parentTransform.scaleY;

    transform.rotation = parentTransform.rotation + local.rotation;

    transform.scaleX = parentTransform.scaleX * local.scaleX;
    transform.scaleY = parentTransform.scaleY * local.scaleY;
}
