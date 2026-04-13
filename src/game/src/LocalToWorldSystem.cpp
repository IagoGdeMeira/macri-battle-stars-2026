#include "../include/LocalToWorldSystem/LocalToWorldSystem.h"

#include "../../domain/components/Position.h"
#include "../../domain/components/LocalPosition.h"
#include "../../domain/components/ParentComponent.h"

#include "../../domain/include/View/View.h"
#include "../../engine/include/UpdateContext/UpdateContext.h"

void LocalToWorldSystem::update(UpdateContext& ctx)
{
    auto& components = ctx.world.components();

    auto view = View<Position, LocalPosition, ParentComponent>(components);

    for (auto [entity, position, local, parent] : view)
    {
        if (!components.has<Position>(parent.parent)) continue;

        const auto& parentPos = components.get<Position>(parent.parent);

        position.x = parentPos.x + local.x;
        position.y = parentPos.y + local.y;
    }
}
