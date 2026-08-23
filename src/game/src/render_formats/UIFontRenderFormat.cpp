#include "UIFontRenderFormat.h"

#include "domain/components/FontEffectsComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/View/View.h"

#include "engine/include/RenderQueue/RenderQueue.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

void UIFontRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    auto view = View<TransformComponent, UILayoutMetricsComponent, UITextComponent, RenderComponent>(comp);
    size_t order = 0;

    for (auto [entity, transform, layout, text, render] : view)
    {
        if (text.text.empty() || !text.font) continue;

        DrawFontCommand cmd = this->buildFontCommand(entity, ctx.world, order++);
        auto& baseCmd = queue.emplace<DrawFontCommand>(std::move(cmd));

        if (comp.has<FontEffectsComponent>(entity))
        {
            const auto& fx = comp.get<FontEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &baseCmd);
        }
    }
}

DrawFontCommand UIFontRenderFormat::buildFontCommand(Entity entity, World& world, size_t order) const
{
    auto& comp = world.components();
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& layout = comp.get<UILayoutMetricsComponent>(entity);
    const auto& text = comp.get<UITextComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    DrawFontCommand cmd;
    cmd.text        = text.text;
    cmd.font        = text.font.get();
    cmd.dest        = Rectangle{transform.position, layout.size};
    cmd.fontSize    = text.fontSize > 0 ? text.fontSize : 16;
    cmd.color       = text.color;
    cmd.layer       = render.layer;
    cmd.zIndex      = render.zIndex;
    cmd.order       = order;
    return cmd;
}
