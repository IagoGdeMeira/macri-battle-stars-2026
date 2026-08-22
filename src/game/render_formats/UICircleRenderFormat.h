#ifndef ui_circle_render_format_h
#define ui_circle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/CircleShapeComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/include/View/View.h"

#include "engine/draw_commands/DrawCircleCommand.h"
#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UICircleRenderFormat : public IRenderFormat
{
public:
    UICircleRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx, RenderQueue& queue) override
    {
        auto& comp = ctx.world.components();
        auto view = View<TransformComponent, CircleShapeComponent, UILayoutMetricsComponent, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, transform, shape, layout, render] : view)
        {
            DrawCircleCommand cmd;
            cmd.circle = shape.circle;
            cmd.color = shape.color;
            cmd.filled = shape.filled;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            auto& baseCmd = queue.emplace<DrawCircleCommand>(std::move(cmd));

            if (comp.has<CircleEffectsComponent>(entity))
            {
                const auto& fx = comp.get<CircleEffectsComponent>(entity);
                for (auto& effect : fx.effects) if (effect) effect(&queue, &baseCmd);
            }
        }
    }

private:
    Renderer& renderer;
};

#endif // ui_circle_render_format
