#ifndef ui_rectangle_render_format_h
#define ui_rectangle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/UITransform.h"
#include "domain/include/View/View.h"

#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UIRectangleRenderFormat : public IRenderFormat
{
public:
    UIRectangleRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx, RenderQueue& queue) override
    {
        auto& comp = ctx.world.components();
        auto view = View<UITransform, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, transform, render] : view)
        {
            auto& cmd = queue.emplace<DrawRectangleCommand>();
            cmd.rect = transform.rect;
            cmd.color = Color::WHITE();
            cmd.filled = false;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (comp.has<RectangleEffectsComponent>(entity))
            {
                const auto& fx = comp.get<RectangleEffectsComponent>(entity);
                for (auto& effect : fx.effects) effect(&queue, &cmd);
            }
        }
    }

private:
    Renderer& renderer;
};

#endif // ui_rectangle_render_format_h
