#include "../../render_formats/WorldTextureRenderFormat.h"

#include <cmath>

void WorldTextureRenderFormat::render(RenderContext& ctx)
{
    this->batch.clear();
    auto view = View<SpriteComponent, TransformComponent, RenderComponent>(ctx.world.components());
    size_t order = 0;

    for (auto [entity, sprite, transform, render] : view)
    {
        if (!sprite.texture) continue;
        DrawTextureCommand cmd = buildTextureCommand(entity, ctx.world, order++);

        if (ctx.world.components().has<VisualEffectsComponent>(entity))
        {
            const auto& fx = ctx.world.components().get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.textureEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }
    this->batch.submit(this->renderer);
}

DrawTextureCommand WorldTextureRenderFormat::buildTextureCommand(Entity& entity, World& world, size_t order) const
{
    auto& components = world.components();
    const auto& sprite = components.get<SpriteComponent>(entity);
    const auto& transform = components.get<TransformComponent>(entity);
    const auto& render = components.get<RenderComponent>(entity);

    const Position parallax = this->resolveParallax(world, entity);
    const Viewport vp = this->getViewport(/*ctx*/);
    const Position screenPos = this->worldToScreen({transform.x, transform.y}, vp, parallax);

    Rectangle spriteConfig = {{ transform.scaleX, transform.scaleY }, sprite.size };

    DrawTextureCommand cmd;
    cmd.texture = sprite.texture.get();
    cmd.dest.position = screenPos;
    cmd.rotation= transform.rotation;
    this->computeSpriteTransform(spriteConfig, cmd);

    if (components.has<OrientationComponent>(entity))
    {
        bool symmetric = components.has<AnimationControllerComponent>(entity)
            ? components.get<AnimationControllerComponent>(entity).animations.symmetric : true;
        cmd.flipX = symmetric ? components.get<OrientationComponent>(entity).direction == Orientation::Left : false;
    }

    cmd.layer = render.layer;
    cmd.zIndex = render.zIndex;
    cmd.order = order;
    cmd.source =
    {
        {static_cast<float>(sprite.source.position.x), static_cast<float>(sprite.source.position.y)},      
        {static_cast<float>(sprite.source.size.width), static_cast<float>(sprite.source.size.height)}
    };
    cmd.useSourceRect = sprite.useSourceRect;
    return cmd;
}
