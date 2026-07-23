#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ParallaxComponent.h"
#include "domain/utils/Logger/Logger.h"

Position WorldRenderUtils::worldToScreen(Camera2D& camera, Position worldPos, Viewport& vp, const Position& parallax)
{
    Position camPos = camera.getPosition();
    float zoom = camera.getZoom();

    float screenX = (worldPos.x - camPos.x * parallax.x) * zoom + vp.width / 2.f;
    float screenY = (worldPos.y - camPos.y * parallax.y) * zoom + vp.height / 2.f;

    LOG_DEBUG("worldToScreen: world=({},{}), screen=({},{})", worldPos.x, worldPos.y, screenX, screenY);
    return {screenX, screenY};
}

Position WorldRenderUtils::resolveParallax(World& world, Entity& entity)
{
    auto& comp = world.components();
    if (comp.has<ParallaxComponent>(entity))
    {
        const auto& parallax = comp.get<ParallaxComponent>(entity);
        LOG_DEBUG("resolveParallax: entity {} has parallax ({}, {})", entity.id, parallax.factor.x, parallax.factor.y);
        return parallax.factor;
    }
    LOG_WARN("resolveParallax: entity {} has NO parallax component, using default (1,1)", entity.id);
    return Position{1.f, 1.f};
}

void WorldRenderUtils::computeSpriteTransform(const Rectangle& spriteConfig, DrawTextureCommand& cmd)
{
    const float width = spriteConfig.size.width * std::abs(spriteConfig.position.x);
    const float height = spriteConfig.size.height * std::abs(spriteConfig.position.y);
    
    cmd.dest.size = { width, height };
    cmd.dest.position.x -= width * 0.5f;
    cmd.dest.position.y -= height * 0.5f;
    cmd.flipX = spriteConfig.position.x < 0.f;
    cmd.flipY = spriteConfig.position.y < 0.f;
}
