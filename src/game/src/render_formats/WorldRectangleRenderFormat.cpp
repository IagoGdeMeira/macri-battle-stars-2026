#include "WorldRectangleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldRectangleRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    
    auto view = View<RectangleShapeComponent, TransformComponent>(comp);

    size_t order = 0;
    for (auto [entity, shape, transform] : view)
    {
        if (comp.has<ActiveComponent>(entity) && !comp.get<ActiveComponent>(entity).active) continue;

        float width = shape.rect.size.width * std::abs(transform.scale.x);
        float height = shape.rect.size.height * std::abs(transform.scale.y);
        if (width <= 0.f || height <= 0.f) continue;

        DrawRectangleCommand cmd = this->buildRectangleCommand(entity, ctx.world, order++);

        if (comp.has<RectangleEffectsComponent>(entity))
        {
            const auto& fx = comp.get<RectangleEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
        }

        queue.emplace<DrawRectangleCommand>(std::move(cmd));
    }
}

DrawRectangleCommand WorldRectangleRenderFormat::buildRectangleCommand(Entity& entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<RectangleShapeComponent>(entity);

    int layer = 0, zIndex = 0;
    if (comp.has<RenderComponent>(entity))
    {
        const auto& render = comp.get<RenderComponent>(entity);
        layer  = render.layer;
        zIndex = render.zIndex;
    }
    else zIndex = 100;

    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position);

    DrawRectangleCommand cmd;
    cmd.rect.position.x = screenPos.x - (shape.rect.size.width * std::abs(transform.scale.x)) * 0.5f;
    cmd.rect.position.y = screenPos.y - (shape.rect.size.height * std::abs(transform.scale.y)) * 0.5f;
    cmd.rect.size.width  = shape.rect.size.width  * std::abs(transform.scale.x);
    cmd.rect.size.height = shape.rect.size.height * std::abs(transform.scale.y);
    cmd.color = shape.color;
    cmd.filled = shape.filled;
    cmd.layer  = layer;
    cmd.zIndex = zIndex;
    cmd.order  = order;
    return cmd;
}
