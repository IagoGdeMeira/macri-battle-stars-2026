#include "WorldRectangleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/RectangleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldRectangleRenderFormat::render(RenderContext& ctx)
{
    this->batch.clear();
    auto& comp = ctx.world.components();
    
    auto view = View<RectangleShapeComponent, TransformComponent>(comp);
    size_t order = 0;

    for (auto [entity, shape, transform] : view)
    {
        float width = shape.rect.size.width * std::abs(transform.scale.x);
        float height = shape.rect.size.height * std::abs(transform.scale.y);
        if (width <= 0.f || height <= 0.f) continue;

        DrawRectangleCommand cmd = this->buildRectangleCommand(entity, ctx.world, order++);
        if (comp.has<VisualEffectsComponent>(entity))
        {
            const auto& fx = comp.get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.rectangleEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }
    this->batch.submit(this->renderer);
}

DrawRectangleCommand WorldRectangleRenderFormat::buildRectangleCommand(Entity& entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<RectangleShapeComponent>(entity);

    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position);
    float zoom = this->camera.getZoom();

    DrawRectangleCommand cmd;
    cmd.rect.position.x = screenPos.x - (shape.rect.size.width * std::abs(transform.scale.x) * zoom) * 0.5f;
    cmd.rect.position.y = screenPos.y - (shape.rect.size.height * std::abs(transform.scale.y) * zoom) * 0.5f;
    cmd.rect.size.width = shape.rect.size.width * std::abs(transform.scale.x) * zoom;
    cmd.rect.size.height = shape.rect.size.height * std::abs(transform.scale.y) * zoom;
    cmd.color = shape.color;
    cmd.filled = shape.filled;
    cmd.order = order;
    return cmd;
}
