#include "UITextureRenderFormat.h"

#include "domain/components/RenderComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

void UITextureRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    auto view = View<UISpriteComponent, TransformComponent, UILayoutMetricsComponent, RenderComponent>(comp);
    size_t order = 0;

    for (auto [entity, sprite, transform, layout, render] : view)
    {
        if (!sprite.texture) continue;

        DrawTextureCommand cmd = this->buildTextureCommand(entity, ctx.world, order++);
        auto& baseCmd = queue.emplace<DrawTextureCommand>(std::move(cmd));

        if (comp.has<TextureEffectsComponent>(entity))
        {
            const auto& fx = comp.get<TextureEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &baseCmd);
        }
    }
}

DrawTextureCommand UITextureRenderFormat::buildTextureCommand(Entity entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& sprite = comp.get<UISpriteComponent>(entity);
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& layout = comp.get<UILayoutMetricsComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    DrawTextureCommand cmd;
    cmd.texture     = sprite.texture;
    cmd.dest        = Rectangle{transform.position, layout.size};
    cmd.rotation    = transform.rotation;
    cmd.flipX       = (transform.scale.x < 0.f);
    cmd.flipY       = (transform.scale.y < 0.f);
    cmd.layer       = render.layer;
    cmd.zIndex      = render.zIndex;
    cmd.order       = order;
    cmd.tint        = sprite.tint;
    cmd.blend       = BlendMode::Normal;
    return cmd;
}
