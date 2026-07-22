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
    using hrclock = std::chrono::high_resolution_clock;
    using milliseconds = std::chrono::milliseconds;
    auto& comp = ctx.world.components();
    
    auto start = hrclock::now();

    this->batch.clear();
    auto view = View<SpriteComponent, TransformComponent, RenderComponent>(comp);
    size_t order = 0;
    size_t entityCount = 0;

    for (auto [entity, sprite, transform, render] : view)
    {
        auto texture = this->resourceManager.load<Texture>(this->textureLoader, sprite.texturePath);
        if (!texture) continue;
        ++entityCount;

        DrawTextureCommand cmd = this->buildTextureCommand(entity, ctx.world, order++, texture);

        LOG_DEBUG("WorldTextureRenderFormat: entity {}, has texture: {}", entity.id, texture ? "yes" : "no");

        auto& srcPos = cmd.source.position;
        auto& srcSize = cmd.source.size;
        auto& destPos = cmd.dest.position;
        auto& destSize = cmd.dest.size;

        LOG_DEBUG("Rendering entity {}: source=({}, {}) size=({}x{}) dest=({}, {}) size=({}x{})",
            entity.id, srcPos.x, srcPos.y, srcSize.width, srcSize.height, destPos.x, destPos.y, destSize.width, destSize.height);

        if (comp.has<VisualEffectsComponent>(entity))
        {
            const auto& fx = comp.get<VisualEffectsComponent>(entity);
            for (auto& effect : fx.textureEffects) effect(this->batch, cmd);
        }
        this->batch.add(cmd);
    }
    this->batch.submit(this->renderer);

    auto end = hrclock::now();
    auto duration = std::chrono::duration_cast<milliseconds>(end - start).count();
    if (duration > 10) LOG_DEBUG("WorldTextureRenderFormat::render took {} ms for {} entities", duration, entityCount);
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
    Viewport vp = this->viewport;

    const Position screenPos = WorldRenderUtils::worldToScreen(this->camera, worldPos, vp, parallax);
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
