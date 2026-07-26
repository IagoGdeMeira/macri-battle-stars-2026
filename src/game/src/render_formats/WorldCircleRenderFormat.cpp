#include "WorldCircleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/CircleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldCircleRenderFormat::render(RenderContext& ctx)
{
    this->batch.clear();
    auto& comp = ctx.world.components();

    auto view = View<CircleShapeComponent, TransformComponent>(comp);
    size_t order = 0;

    for (auto [entity, shape, transform] : view)
    {
        if (shape.circle.radius <= 0.f) continue;

        DrawCircleCommand cmd = this->buildCircleCommand(entity, ctx.world, order++);
        if (comp.has<VisualEffectsComponent>(entity))
        {
            const auto& fx = comp.get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.circleEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }
    this->batch.submit(this->renderer);
}

DrawCircleCommand WorldCircleRenderFormat::buildCircleCommand(Entity& entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<CircleShapeComponent>(entity);

    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position);
    float zoom = this->camera.getZoom();

    DrawCircleCommand cmd;
    cmd.circle.position = screenPos;
    cmd.circle.radius = shape.circle.radius * std::max(std::abs(transform.scale.x), std::abs(transform.scale.y)) * zoom;
    cmd.color = shape.color;
    cmd.filled = shape.filled;
    cmd.order = order;
    return cmd;
}
