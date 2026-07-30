#include "LocalToWorldSystem/LocalToWorldSystem.h"

#include "domain/components/CircleShapeComponent.h"
#include "domain/components/LocalTransform.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RectangleShapeComponent.h"
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
        this->applyParentTransform({transform, parentTransform, local});
    }
}

float LocalToWorldSystem::rotateLocalX(const LocalTransform& local, float cosR, float sinR)
{ return local.position.x * cosR - local.position.y * sinR; }

float LocalToWorldSystem::rotateLocalY(const LocalTransform& local, float cosR, float sinR)
{ return local.position.x * sinR + local.position.y * cosR; }

void LocalToWorldSystem::applyParentTransform(const ParentParams& params)
{
    auto& transform = params.transform;
    const auto& local = params.local;
    const auto& parentTrans = params.parentTrans;

    float cosR = std::cos(parentTrans.rotation);
    float sinR = std::sin(parentTrans.rotation);

    float rotatedX = LocalToWorldSystem::rotateLocalX(local, cosR, sinR);
    float rotatedY = LocalToWorldSystem::rotateLocalY(local, cosR, sinR);

    auto& parentPos = parentTrans.position;
    auto& parentScale = parentTrans.scale;
    transform.position.x = parentPos.x + rotatedX * parentScale.x;
    transform.position.y = parentPos.y + rotatedY * parentScale.y;
    transform.rotation = parentTrans.rotation + local.rotation;
    transform.scale = { parentScale.x * local.scale.x, parentScale.y * local.scale.y };
}
