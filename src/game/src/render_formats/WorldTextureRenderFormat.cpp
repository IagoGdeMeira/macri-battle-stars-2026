#include "WorldTextureRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/TextureEffectsComponent.h"
#include "domain/resources/Texture/Texture.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldTextureRenderFormat::render(RenderContext& ctx, RenderQueue& queue)
{
    auto& comp = ctx.world.components();
    
    auto view = View<SpriteComponent, TransformComponent, RenderComponent>(comp);

    size_t order = 0;
    for (auto [entity, sprite, transform, render] : view)
    {
        if (!sprite.cachedTexture)
        { sprite.cachedTexture = this->resourceManager.load<Texture>(this->textureLoader, sprite.texturePath); }

        if (!sprite.cachedTexture) continue;
        DrawTextureCommand cmd = this->buildTextureCommand(entity, ctx.world, order++, sprite.cachedTexture);

        if (comp.has<TextureEffectsComponent>(entity))
        {
            const auto& fx = comp.get<TextureEffectsComponent>(entity);
            for (auto& effect : fx.effects) if (effect) effect(&queue, &cmd);
        }

        queue.emplace<DrawTextureCommand>(std::move(cmd));
    }
}

DrawTextureCommand WorldTextureRenderFormat::buildTextureCommand(
    Entity& entity, World& world, size_t order, std::shared_ptr<Texture> texture
) const {
    auto& comp = world.components();
    const auto& sprite = comp.get<SpriteComponent>(entity);
    const auto& transform = comp.get<TransformComponent>(entity);
    const auto& render = comp.get<RenderComponent>(entity);

    const Position parallax = WorldRenderUtils::resolveParallax(world, entity);
    Position worldPos{transform.position.x, transform.position.y};

    const Position screenPos = WorldRenderUtils::worldToScreen(this->camera, worldPos, parallax);
    Rectangle spriteConfig = {{transform.scale.x, transform.scale.y}, sprite.size};

    DrawTextureCommand cmd;
    cmd.texture = texture;
    cmd.dest.position = screenPos;
    cmd.rotation = transform.rotation;
    WorldRenderUtils::computeSpriteTransform(spriteConfig, cmd);

    bool hasOrientation = false, orientFlipX = false;
    if (comp.has<ParentComponent>(entity))
    {
        Entity parent = comp.get<ParentComponent>(entity).parent;
        if (comp.has<OrientationComponent>(parent))
        {
            orientFlipX = this->shouldFlipTexture(parent, world);
            hasOrientation = true;
        }
    }
    else if (comp.has<OrientationComponent>(entity))
    {
        orientFlipX = this->shouldFlipTexture(entity, world);
        hasOrientation = true;
    }
    if (hasOrientation) cmd.flipX = orientFlipX;

    float oldWidth  = cmd.dest.size.width;
    float oldHeight = cmd.dest.size.height;
    cmd.dest.size.width  *= this->camera.getZoom();
    cmd.dest.size.height *= this->camera.getZoom();
    cmd.dest.position.x -= (cmd.dest.size.width  - oldWidth)  * 0.5f;
    cmd.dest.position.y -= (cmd.dest.size.height - oldHeight) * 0.5f;

    cmd.layer = render.layer;
    cmd.zIndex = render.zIndex;
    cmd.order = order;
    cmd.source.position = sprite.source.position;
    cmd.source.size = sprite.source.size;
    cmd.useSourceRect = sprite.useSourceRect;
    return cmd;
}

bool WorldTextureRenderFormat::shouldFlipTexture(Entity& entity, World& world) const
{
    auto& comp = world.components();

    bool symmetric = comp.has<AnimationControllerComponent>(entity)
        ? comp.get<AnimationControllerComponent>(entity).animations.symmetric : true;
    return symmetric ? (comp.get<OrientationComponent>(entity).direction == Orientation::Left) : false;
}
