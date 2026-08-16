#ifndef ui_circle_render_format_h
#define ui_circle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/CircleEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
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
        auto view = View<TransformComponent, UIRectComponent, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, transform, uiRect, render] : view)
        {
            auto& cmd = queue.emplace<DrawCircleCommand>();
            cmd.circle.position = {
                transform.position.x + uiRect.size.width * 0.5f,
                transform.position.y + uiRect.size.height * 0.5f
            };
            cmd.circle.radius = std::min(uiRect.size.width, uiRect.size.height) * 0.5f;
            cmd.color = Color::WHITE();
            cmd.filled = false;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (comp.has<CircleEffectsComponent>(entity))
            {
                const auto& fx = comp.get<CircleEffectsComponent>(entity);
                for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
            }
        }
    }

private:
    Renderer& renderer;
};

#endif // ui_circle_render_format_h
