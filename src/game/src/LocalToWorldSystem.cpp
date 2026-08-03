#include "LocalToWorldSystem/LocalToWorldSystem.h"

#include "domain/components/ChildrenComponent.h"
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
    auto view = View<TransformComponent>(comp);

    for (auto [entity, transform] : view) if (!comp.has<ParentComponent>(entity) && comp.has<ChildrenComponent>(entity))
    { LocalToWorldSystem::updateTransformRecursive(ctx, entity, transform); }
}

void LocalToWorldSystem::updateTransformRecursive(UpdateContext& ctx, Entity parent, const TransformComponent& parentTrans)
{
    auto& comp = ctx.world.components();

    auto& childrenComp = comp.get<ChildrenComponent>(parent);
    if (childrenComp.children.empty()) return;

    for (Entity child : childrenComp.children)
    {
        if (!comp.has<TransformComponent>(child) || !comp.has<LocalTransform>(child)) continue;
        
        auto& childTrans = comp.get<TransformComponent>(child);
        auto& childLocal = comp.get<LocalTransform>(child);
        
        float rad = parentTrans.rotation * (3.14159265f / 180.f);
        float cosA = std::cos(rad), sinA = std::sin(rad);

        float rotatedX = childLocal.position.x * cosA - childLocal.position.y * sinA;
        float rotatedY = childLocal.position.x * sinA + childLocal.position.y * cosA;

        childTrans.position.x = parentTrans.position.x + rotatedX * parentTrans.scale.x;
        childTrans.position.y = parentTrans.position.y + rotatedY * parentTrans.scale.y;
        childTrans.rotation = parentTrans.rotation + childLocal.rotation;
        childTrans.scale = { parentTrans.scale.x * childLocal.scale.x, parentTrans.scale.y * childLocal.scale.y };

        if (comp.has<ChildrenComponent>(child)) LocalToWorldSystem::updateTransformRecursive(ctx, child, childTrans);
    }
}
