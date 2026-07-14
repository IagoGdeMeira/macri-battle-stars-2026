#include "../include/WorldRenderUtils/WorldRenderUtils.h"

#include "../../domain/components/ParallaxComponent.h"

Position WorldRenderUtils::worldToScreen(Camera2D& camera, Position worldPos, Viewport& vp, const Position& parallax)
{
    Position camPos = camera.getPosition();
    float zoom = camera.getZoom();

    float screenX = (worldPos.x - camPos.x * parallax.x) * zoom + vp.width / 2.f;
    float screenY = (worldPos.y - camPos.y * parallax.y) * zoom + vp.height / 2.f;

    return {screenX, screenY};
}

Position WorldRenderUtils::resolveParallax(World& world, Entity& entity)
{
    auto& comp = world.components();
    if (comp.has<ParallaxComponent>(entity))
    {
        const auto& parallax = comp.get<ParallaxComponent>(entity);
        return parallax.factor;
    }
    return Position{1.f, 1.f};
}

void WorldRenderUtils::computeSpriteTransform(const Camera2D& camera, const Rectangle& spriteConfig, DrawTextureCommand& cmd)
{
    const float zoom = camera.getZoom();
    const float width = spriteConfig.size.width * std::abs(spriteConfig.position.x) * zoom;
    const float height = spriteConfig.size.height * std::abs(spriteConfig.position.y) * zoom;
    cmd.dest.size = { width, height };
    cmd.flipX = spriteConfig.position.x < 0.f;
    cmd.flipY = spriteConfig.position.y < 0.f;
}
