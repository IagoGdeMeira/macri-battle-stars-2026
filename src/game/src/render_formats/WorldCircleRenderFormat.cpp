#include "WorldCircleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/RenderComponent.h"
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
        if (comp.has<ActiveComponent>(entity) && !comp.get<ActiveComponent>(entity).active) continue;
        if (shape.circle.radius <= 0.f) continue;

        DrawCircleCommand cmd = this->buildCircleCommand(entity, ctx.world, order++);
        if (comp.has<VisualEffectsComponent>(entity))
        {
            const auto& fx = comp.get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.circleEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }
}

std::vector<const DrawCommand*> WorldCircleRenderFormat::collectCommands() const
{
    std::vector<const DrawCommand*> cmds;
    for (const auto& cmd : this->batch.getCommands()) cmds.push_back(&cmd);
    return cmds;
}

DrawCircleCommand WorldCircleRenderFormat::buildCircleCommand(Entity& entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<CircleShapeComponent>(entity);

    int layer = 0;
    int zIndex = 0;
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
    cmd.color = shape.color;
    cmd.filled = shape.filled;
    cmd.layer  = layer;
    cmd.zIndex = zIndex;
    cmd.order  = order;
    return cmd;
}
