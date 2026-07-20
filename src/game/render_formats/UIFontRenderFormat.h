#ifndef ui_font_render_format_h
#define ui_font_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/components/UITransform.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/DrawBatch/DrawFontBatch.h"
#include "engine/include/DrawCommands/DrawCommands.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UIFontRenderFormat : public IRenderFormat
{
public:
    UIFontRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx) override
    {
        this->batch.clear();
        auto& comp = ctx.world.components();
        auto view = View<UITransform, UITextComponent, RenderComponent>(comp);
        size_t order = 0;
        
        for (auto [entity, transform, text, render] : view)
        {
            if (text.text.empty() || !text.font) continue;

            DrawFontCommand cmd;
            cmd.text = text.text;
            cmd.font = text.font.get();
            cmd.dest = transform.rect;
            cmd.fontSize = static_cast<int>(text.fontSize > 0 ? text.fontSize : 16);
            cmd.color = text.color;
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;

            if (comp.has<VisualEffectsComponent>(entity))
            {
                const auto& fx = comp.get<VisualEffectsComponent>(entity);
                for (auto& effect : fx.fontEffects) effect(this->batch, cmd);
            }
            this->batch.add(cmd);
        }
        this->batch.submit(this->renderer);
    }

    void setViewport(const Viewport& vp) override { (void)vp; }

private:
    DrawFontBatch batch;
    Renderer& renderer;
};

#endif // ui_font_render_format_h
