#ifndef ui_texture_render_format_h
#define ui_texture_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/include/View/View.h"

#include "engine/draw_commands/DrawTextureCommand.h"
#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

class UITextureRenderFormat : public IRenderFormat
{
public:
    UITextureRenderFormat(Renderer& renderer) : renderer(renderer) {}

    void render(RenderContext& ctx, RenderQueue& queue) override
    {
        auto& comp = ctx.world.components();
        auto view = View<UISpriteComponent, TransformComponent, UIRectComponent, RenderComponent>(comp);
        size_t order = 0;

        for (auto [entity, sprite, transform, uiRect, render] : view)
        {
            if (!sprite.texture) continue;

            Rectangle dest{transform.position, uiRect.size};

            auto& cmd = queue.emplace<DrawTextureCommand>();
            cmd.texture = sprite.texture;
            cmd.dest = dest;
            cmd.rotation = transform.rotation;
            cmd.flipX = (transform.scale.x < 0.f);
            cmd.flipY = (transform.scale.y < 0.f);
            cmd.layer = render.layer;
            cmd.zIndex = render.zIndex;
            cmd.order = order++;
            cmd.tint = sprite.tint;
            cmd.blend = BlendMode::Normal;

            if (comp.has<TextureEffectsComponent>(entity))
            {
                const auto& fx = comp.get<TextureEffectsComponent>(entity);
                for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
            }
        }
    }

private:
    Renderer& renderer;
};

#endif // ui_texture_render_format_h
