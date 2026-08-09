#include "WorldRenderUtils/WorldRenderUtils.h"

#include "domain/components/ParallaxComponent.h"
#include "domain/components/ParentComponent.h"

#include "engine/value_objects/GameConstants/GameConstants.h"

Position WorldRenderUtils::worldToScreen(Camera2D& camera, Position worldPos, const Position& parallax)
{
    Position camPos = camera.getPosition();
    float zoom = camera.getZoom();
    const Dimension2D& virtualSize = GameConstants::VIRTUAL_SIZE;
    float screenX = (worldPos.x - camPos.x * parallax.x) * zoom + virtualSize.width / 2.f;
    float screenY = (worldPos.y - camPos.y * parallax.y) * zoom + virtualSize.height / 2.f;
    return {screenX, screenY};
}

Position WorldRenderUtils::resolveParallax(World& world, Entity& entity)
{
    auto& comp = world.components();
    Entity current = entity;

    while (true)
    {
        if (comp.has<ParallaxComponent>(current))
        {
            const auto& parallax = comp.get<ParallaxComponent>(current);
            return parallax.factor;
        }
        if (!comp.has<ParentComponent>(current)) break;
        current = comp.get<ParentComponent>(current).parent;
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
