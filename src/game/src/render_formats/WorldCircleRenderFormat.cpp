#include "WorldCircleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldCircleRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    
    auto view = View<CircleShapeComponent, TransformComponent>(comp);

    size_t order = 0;
    for (auto [entity, shape, transform] : view)
    {
        if (comp.has<ActiveComponent>(entity) && !comp.get<ActiveComponent>(entity).active) continue;
        if (shape.circle.radius <= 0.f) continue;

        DrawCircleCommand cmd = this->buildCircleCommand(entity, ctx.world, order++);

        if (comp.has<CircleEffectsComponent>(entity))
        {
            const auto& fx = comp.get<CircleEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
        }

        queue.emplace<DrawCircleCommand>(std::move(cmd));
    }
}

DrawCircleCommand WorldCircleRenderFormat::buildCircleCommand(Entity& entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<CircleShapeComponent>(entity);

    int layer = 0, zIndex = 0;
    if (comp.has<RenderComponent>(entity))
    {
        const auto& render = comp.get<RenderComponent>(entity);
        layer  = render.layer;
        zIndex = render.zIndex;
    }
    else zIndex = 100;

    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position);

    DrawCircleCommand cmd;
    cmd.circle.position = screenPos;
    cmd.circle.radius = shape.circle.radius * std::max(std::abs(transform.scale.x), std::abs(transform.scale.y));
    cmd.circle.radius *= this->camera.getZoom();
    cmd.color   = shape.color;
    cmd.filled  = shape.filled;
    cmd.layer   = layer;
    cmd.zIndex  = zIndex;
    cmd.order   = order;
    return cmd;
}
