#ifndef ui_rectangle_render_format_h
#define ui_rectangle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/UITransform.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"

#include "engine/draw_batches/DrawRectangleBatch.h"
#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UIRectangleRenderFormat : public IRenderFormat
{
public:
    UIRectangleRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx) override
    {
        this->batch.clear();
        auto& comp = ctx.world.components();
        auto view = View<UITransform, RenderComponent>(comp);
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

            if (comp.has<VisualEffectsComponent>(entity))
            {
                const auto& fx = comp.get<VisualEffectsComponent>(entity);
                for (auto& effect : fx.rectangleEffects) effect(this->batch, cmd);
            }
            this->batch.add(cmd);
        }
    }

    std::vector<const DrawCommand*> collectCommands() const override
    {
        std::vector<const DrawCommand*> cmds;
        for (const auto& cmd : this->batch.getCommands()) cmds.push_back(&cmd);
        return cmds;
    }

private:
    DrawRectangleBatch batch;
    Renderer& renderer;
};

#endif // ui_rectangle_render_format_h
