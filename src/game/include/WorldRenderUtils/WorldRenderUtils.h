#ifndef world_render_utils_h
#define world_render_utils_h

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/DrawCommands/DrawCommands.h"
#include "engine/value_objects/Camera2D/Camera2D.h"
#include "engine/value_objects/Viewport/Viewport.h"

class WorldRenderUtils
{
public:
    static Position worldToScreen(Camera2D& camera, Position worldPos, Viewport& vp, const Position& parallax = {1.f, 1.f});
    static Position resolveParallax(World& world, Entity& entity);
    static void computeSpriteTransform(const Camera2D& camera, const Rectangle& spriteConfig, DrawTextureCommand& cmd);
};

#endif // world_render_utils_h
