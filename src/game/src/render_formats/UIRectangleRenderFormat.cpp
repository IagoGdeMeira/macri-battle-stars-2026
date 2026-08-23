#include "UIRectangleRenderFormat.h"

#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

void UIRectangleRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, UILayoutMetricsComponent, RenderComponent>(comp);
    size_t order = 0;

    for (auto [entity, transform, layout, render] : view)
    {
        DrawRectangleCommand cmd = this->buildRectangleCommand(entity, ctx.world, order++);
        if (cmd.rect.size.width <= 0.f || cmd.rect.size.height <= 0.f) continue;

        auto& baseCmd = queue.emplace<DrawRectangleCommand>(std::move(cmd));

        if (comp.has<RectangleEffectsComponent>(entity))
        {
            const auto& fx = comp.get<RectangleEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &baseCmd);
        }
    }
}

DrawRectangleCommand UIRectangleRenderFormat::buildRectangleCommand(Entity entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& layout = comp.get<UILayoutMetricsComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    DrawRectangleCommand cmd;
    cmd.rect.position   = transform.position;
    cmd.rect.size       = layout.size;
    cmd.layer           = render.layer;
    cmd.zIndex          = render.zIndex;
    cmd.order           = order;

    if (comp.has<RectangleShapeComponent>(entity))
    {
        const auto& shape = comp.get<RectangleShapeComponent>(entity);
        cmd.color = shape.color;
        cmd.filled = shape.filled;
    }
    else
    {
        cmd.color = Color::WHITE();
        cmd.filled = false;
    }

    return cmd;
}
