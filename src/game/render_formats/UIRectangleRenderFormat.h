#ifndef ui_rectangle_render_format_h
#define ui_rectangle_render_format_h

#include "../include/IRenderFormat/IRenderFormat.h"

#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/UITransform.h"
#include "../../domain/components/VisualEffectsComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/DrawBatch/DrawRectangleBatch.h"
#include "../../engine/include/DrawCommands/DrawCommands.h"
#include "../../engine/include/RenderContext/RenderContext.h"
#include "../../engine/include/Renderer/Renderer.h"

class UIRectangleRenderFormat : public IRenderFormat
{
public:
    UIRectangleRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx) override
    {
        this->batch.clear();
        auto view = View<UITransform, RenderComponent>(ctx.world.components());
        size_t order = 0;
        
        for (auto [entity, transform, render] : view)
        {
            DrawRectangleCommand cmd;
            cmd.rect = transform.rect;
            cmd.color = Color::WHITE();
            cmd.filled = false;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (ctx.world.components().has<VisualEffectsComponent>(entity))
            {
                const auto& fx = ctx.world.components().get<VisualEffectsComponent>(entity);
                for (auto& effect : fx.rectangleEffects) effect(this->batch, cmd);
            }
            this->batch.add(cmd);
        }
        this->batch.submit(this->renderer);
    }

private:
    DrawRectangleBatch batch;
    Renderer& renderer;
};

#endif // ui_rectangle_render_format_h
