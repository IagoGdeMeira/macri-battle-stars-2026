#include "WorldCircleRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ShapeRenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"
#include "domain/value_objects/CollisionFrame/ColliderDef.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldCircleRenderFormat::render(RenderContext& ctx)
{
    this->batch.clear();
    auto view = View<ShapeRenderComponent, TransformComponent>(ctx.world.components());
    size_t order = 0;
    for (auto [entity, shapeComp, transform] : view)
    {
        if (!shapeComp.shape) continue;
        if (shapeComp.shape->getType() != ColliderDef::ColliderType::Circle) continue;

        DrawCircleCommand cmd = this->buildCircleCommand(entity, ctx.world, order++);
        if (ctx.world.components().has<VisualEffectsComponent>(entity))
        {
            const auto& fx = ctx.world.components().get<VisualEffectsComponent>(entity);
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
    const auto& shapeComp = comp.get<ShapeRenderComponent>(entity);
    auto* circ = static_cast<const CircleDef*>(shapeComp.shape.get());

    Viewport vp = this->viewport;
    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position);
    float zoom = this->camera.getZoom();

    DrawCircleCommand cmd;
    cmd.circle.position = screenPos;
    cmd.circle.radius = circ->radius * std::max(std::abs(transform.scale.x), std::abs(transform.scale.y)) * zoom;
    cmd.color = shapeComp.color;
    cmd.filled = shapeComp.filled;
    cmd.order = order;
    return cmd;
}
