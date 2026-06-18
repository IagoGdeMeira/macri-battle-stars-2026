#include "../render_formats/WorldRectangleRenderFormat.h"

#include "../include/WorldRenderUtils/WorldRenderUtils.h"

#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/ShapeRenderComponent.h"
#include "../../domain/components/TransformComponent.h"
#include "../../domain/components/VisualEffectsComponent.h"
#include "../../domain/include/View/View.h"
#include "../../domain/value_objects/CollisionFrame/ColliderDef.h"

#include "../../engine/include/RenderContext/RenderContext.h"

void WorldRectangleRenderFormat::render(RenderContext& ctx)
{
    this->batch.clear();
    auto view = View<ShapeRenderComponent, TransformComponent>(ctx.world.components());
    size_t order = 0;

    for (auto [entity, shapeComp, transform] : view)
    {
        if (!shapeComp.shape) continue;
        if (shapeComp.shape->getType() != ColliderDef::ColliderType::Rectangle) continue;

        DrawRectangleCommand cmd = this->buildRectangleCommand(entity, ctx.world, order++);
        if (ctx.world.components().has<VisualEffectsComponent>(entity))
        {
            const auto& fx = ctx.world.components().get<VisualEffectsComponent>(entity);
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
    const auto& shapeComp = comp.get<ShapeRenderComponent>(entity);
    auto* rect = static_cast<const RectangleDef*>(shapeComp.shape.get());

    Viewport vp = this->viewport;
    Position screenPos = WorldRenderUtils::worldToScreen(this->camera, transform.position, vp);
    float zoom = this->camera.getZoom();

    DrawRectangleCommand cmd;
    cmd.rect.position.x = screenPos.x - (rect->width * std::abs(transform.scale.x) * zoom) * 0.5f;
    cmd.rect.position.y = screenPos.y - (rect->height * std::abs(transform.scale.y) * zoom) * 0.5f;
    cmd.rect.size.width = rect->width * std::abs(transform.scale.x) * zoom;
    cmd.rect.size.height = rect->height * std::abs(transform.scale.y) * zoom;
    cmd.color = shapeComp.color;
    cmd.filled = shapeComp.filled;
    cmd.order = order;
    return cmd;
}
