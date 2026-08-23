#include "UICircleRenderFormat.h"

#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

void UICircleRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, CircleShapeComponent, UILayoutMetricsComponent, RenderComponent>(comp);
    size_t order = 0;

    for (auto [entity, transform, shape, layout, render] : view)
    {
        DrawCircleCommand cmd = this->buildCircleCommand(entity, ctx.world, order++);
        auto& baseCmd = queue.emplace<DrawCircleCommand>(std::move(cmd));

        if (comp.has<CircleEffectsComponent>(entity))
        {
            const auto& fx = comp.get<CircleEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &baseCmd);
        }
    }
}

DrawCircleCommand UICircleRenderFormat::buildCircleCommand(Entity entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& shape = comp.get<CircleShapeComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    DrawCircleCommand cmd;
    cmd.circle  = shape.circle;
    cmd.color   = shape.color;
    cmd.filled  = shape.filled;
    cmd.layer   = render.layer;
    cmd.zIndex  = render.zIndex;
    cmd.order   = order;
    return cmd;
}
