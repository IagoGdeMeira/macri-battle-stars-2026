#include "LocalToWorldSystem/LocalToWorldSystem.h"

#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/UpdateContext/UpdateContext.h"

#include <cmath>

void LocalToWorldSystem::update(UpdateContext& ctx)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, LocalTransform, ParentComponent>(comp);
    for (auto [entity, transform, local, parent] : view)
    {
        if (!comp.has<TransformComponent>(parent.parent)) continue;
        const auto& parentTransform = comp.get<TransformComponent>(parent.parent);
        this->applyParentTransform(transform, local, parentTransform);

        /*
        LOG_DEBUG("LocalToWorld: entity {} parent {} local({},{}) -> world({},{})",
            entity.id, parent.parent.id, local.position.x, local.position.y,
            transform.position.x, transform.position.y);
        */
    }
}

float LocalToWorldSystem::rotateLocalX(const LocalTransform& local, float cosR, float sinR)
{ return local.position.x * cosR - local.position.y * sinR; }

float LocalToWorldSystem::rotateLocalY(const LocalTransform& local, float cosR, float sinR)
{ return local.position.x * sinR + local.position.y * cosR; }

void LocalToWorldSystem::applyParentTransform(
    TransformComponent& transform, const LocalTransform& local, const TransformComponent& parentTransform
) {
    float cosR = std::cos(parentTransform.rotation);
    float sinR = std::sin(parentTransform.rotation);

    float rotatedX = LocalToWorldSystem::rotateLocalX(local, cosR, sinR);
    float rotatedY = LocalToWorldSystem::rotateLocalY(local, cosR, sinR);

    auto& parentPos = parentTransform.position;
    auto& parentScale = parentTransform.scale;
    transform.position = { parentPos.x + rotatedX * parentScale.x, parentPos.y + rotatedY * parentScale.y };
    transform.rotation = parentTransform.rotation + local.rotation;
    transform.scale = { parentScale.x * local.scale.x, parentScale.y * local.scale.y };
}
