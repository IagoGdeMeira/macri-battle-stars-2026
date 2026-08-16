#ifndef ui_rectangle_render_format_h
#define ui_rectangle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RectangleEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
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
        auto view = View<TransformComponent, UIRectComponent, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, transform, uiRect, render] : view)
        {
            Rectangle rect{transform.position, uiRect.size};

            auto& cmd = queue.emplace<DrawRectangleCommand>();
            cmd.rect = rect;
            cmd.color = Color::WHITE();
            cmd.filled = false;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (comp.has<RectangleEffectsComponent>(entity))
            {
                const auto& fx = comp.get<RectangleEffectsComponent>(entity);
                for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
            }
        }
    }

private:
    Renderer& renderer;
};

#endif // ui_rectangle_render_format_h
