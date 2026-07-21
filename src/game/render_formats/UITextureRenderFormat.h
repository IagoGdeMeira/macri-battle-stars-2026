#ifndef ui_texture_render_format_h
#define ui_texture_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITransform.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/DrawBatch/DrawTextureBatch.h"
#include "engine/include/DrawCommands/DrawCommands.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UITextureRenderFormat : public IRenderFormat
{
public:
    UITextureRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx) override
    {
        this->batch.clear();
        auto& comp = ctx.world.components();
        auto view = View<UISpriteComponent, UITransform, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, sprite, transform, render] : view)
        {
            if (!sprite.texture) continue;

            DrawTextureCommand cmd;
            cmd.texture = sprite.texture;
            cmd.dest = transform.rect;
            cmd.rotation = transform.rotation;
            cmd.flipX = (transform.scale.x < 0.f);
            cmd.flipY = (transform.scale.y < 0.f);
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;
            cmd.tint = sprite.tint;
            cmd.blend = BlendMode::Normal;

            if (comp.has<VisualEffectsComponent>(entity))
            {
                const auto& fx = comp.get<VisualEffectsComponent>(entity);
                for (auto& effect : fx.textureEffects) effect(this->batch, cmd);
            }
            this->batch.add(cmd);
        }
        this->batch.submit(this->renderer);
    }

    void setViewport(const Viewport& vp) override { (void)vp; }

private:
    DrawTextureBatch batch;
    Renderer& renderer;
};

#endif // ui_texture_render_format_h
