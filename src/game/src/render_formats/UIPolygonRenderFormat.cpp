#include "UIPolygonRenderFormat.h"

#include "domain/components/PolygonShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

UIPolygonRenderFormat::UIPolygonRenderFormat(Renderer& renderer) : renderer(renderer) {}

void UIPolygonRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, PolygonShapeComponent, RenderComponent>(comp);
    size_t order = 0;

    for (auto [entity, transform, shape, render] : view)
    {
        DrawPolygonCommand cmd = this->buildPolygonCommand(entity, ctx.world, order++);
        if (cmd.polygon.vertices.size() < 3) continue;
        queue.emplace<DrawPolygonCommand>(std::move(cmd));
    }
}

DrawPolygonCommand UIPolygonRenderFormat::buildPolygonCommand(Entity entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& shape = comp.get<PolygonShapeComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    DrawPolygonCommand cmd;
    cmd.polygon.vertices.reserve(shape.polygon.vertices.size());
    for (const auto& v : shape.polygon.vertices) cmd.polygon.vertices.push_back(Position{
        transform.position.x + v.x,
        transform.position.y + v.y
    });
    cmd.color = shape.color;
    cmd.filled = shape.filled;
    cmd.layer = render.layer;
    cmd.zIndex = render.zIndex;
    cmd.order = order;
    return cmd;
}
