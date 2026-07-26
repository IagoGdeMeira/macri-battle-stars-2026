#include "WorldTextureRenderFormat.h"

#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/AnimationControllerComponent.h"
#include "domain/components/OrientationComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/SpriteComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/VisualEffectsComponent.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/resources/Texture/Texture.h"
#include "engine/value_objects/RenderContext/RenderContext.h"

void WorldTextureRenderFormat::render(RenderContext& ctx)
{
    LOG_DEBUG("WorldTextureRenderFormat::render: start");
    auto& comp = ctx.world.components();
    auto view = View<SpriteComponent, TransformComponent, RenderComponent>(comp);
    LOG_DEBUG("WorldTextureRenderFormat: found {} entities with SpriteComponent", view.size());

    this->batch.clear();
    size_t order = 0, entityCount = 0;
    for (auto [entity, sprite, transform, render] : view)
    {
        LOG_DEBUG("WorldTextureRenderFormat: processing entity {}", entity.id);
        if (!sprite.cachedTexture) sprite.cachedTexture = this->resourceManager.load<Texture>(this->textureLoader, sprite.texturePath);
        
        auto texture = sprite.cachedTexture;
        if (!texture)
        {
            LOG_WARN("WorldTextureRenderFormat: texture not loaded for entity {}", entity.id);
            continue;
        }
        ++entityCount;

        DrawTextureCommand cmd = this->buildTextureCommand(entity, ctx.world, order++, texture);
        LOG_DEBUG("WorldTextureRenderFormat: entity {} texture loaded", entity.id);

        if (comp.has<VisualEffectsComponent>(entity))
        {
            const auto& fx = comp.get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.textureEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }

    LOG_DEBUG("WorldTextureRenderFormat: submitting batch with {} commands", entityCount);
    this->batch.submit(this->renderer);
    LOG_DEBUG("WorldTextureRenderFormat: batch submitted");
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
    
    if (comp.has<OrientationComponent>(entity))
    {
        bool symmetric = comp.has<AnimationControllerComponent>(entity)
            ? comp.get<AnimationControllerComponent>(entity).animations.symmetric : true;
        cmd.flipX = symmetric ? (comp.get<OrientationComponent>(entity).direction == Orientation::Left) : false;
    }

    LOG_DEBUG("buildTextureCommand: entity {} worldPos=({},{})", entity.id, worldPos.x, worldPos.y);

    cmd.layer = render.layer;
    cmd.zIndex = render.zIndex;
    cmd.order = order;
    cmd.source.position = sprite.source.position;
    cmd.source.size = sprite.source.size;
    cmd.useSourceRect = sprite.useSourceRect;
    return cmd;
}
