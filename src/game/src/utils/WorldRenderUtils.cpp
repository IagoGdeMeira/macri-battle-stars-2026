#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ParallaxComponent.h"

#include "engine/value_objects/GameConstants/GameConstants.h"

Position WorldRenderUtils::worldToScreen(Camera2D& camera, Position worldPos, const Position& parallax)
{
    Position camPos = camera.getPosition();
    const Dimension2D& virtualSize = GameConstants::VIRTUAL_SIZE;
    float screenX = (worldPos.x - camPos.x * parallax.x) + virtualSize.width / 2.f;
    float screenY = (worldPos.y - camPos.y * parallax.y) + virtualSize.height / 2.f;
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
