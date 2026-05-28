#ifndef ui_circle_render_format_h
#define ui_circle_render_format_h

#include "../include/IRenderFormat/IRenderFormat.h"

#include "../../domain/components/RenderComponent.h"
#include "../../domain/components/UITransform.h"
#include "../../domain/components/VisualEffectsComponent.h"
#include "../../domain/include/View/View.h"

#include "../../engine/include/DrawBatch/DrawCircleBatch.h"
#include "../../engine/include/DrawCommands/DrawCommands.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/RenderContext/RenderContext.h"

class UICircleRenderFormat : public IRenderFormat
{
public:
    UICircleRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx) override
    {
        this->batch.clear();
        auto view = View<UITransform, RenderComponent>(ctx.world.components());
        size_t order = 0;

        for (auto [entity, transform, render] : view)
        {
            DrawCircleCommand cmd;
            cmd.circle.position =
            {
                transform.rect.position.x + transform.rect.size.width * 0.5f,
                transform.rect.position.y + transform.rect.size.height * 0.5f
            };
            cmd.circle.radius = std::min(transform.rect.size.width, transform.rect.size.height) * 0.5f;
            cmd.color = Color::WHITE();
            cmd.filled = false;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (ctx.world.components().has<VisualEffectsComponent>(entity))
            {
                const auto& fx = ctx.world.components().get<VisualEffectsComponent>(entity);
                for (auto& effect : fx.circleEffects) effect(this->batch, cmd);
            }
            this->batch.add(cmd);
        }
        this->batch.submit(this->renderer);
    }

    void setViewport(const Viewport& vp) override { (void)vp; }

private:
    DrawCircleBatch batch;
    Renderer& renderer;
};

#endif // ui_circle_render_format_h
