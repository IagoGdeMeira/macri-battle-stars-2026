#ifndef world_drawer_h
#define world_drawer_h

#include "../Camera2D/Camera2D.h"

#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/Drawer/Drawer.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Viewport/Viewport.h"
#include "../../engine/include/DrawBatch/DrawCircleBatch.h"
#include "../../engine/include/DrawBatch/DrawRectangleBatch.h"
#include "../../engine/include/DrawBatch/DrawTextureBatch.h"

#include <cstddef>
#include <vector>

struct Entity;

class WorldDrawer : public Drawer
{
public:
    WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera);
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Camera2D& camera;

    DrawCircleBatch circleBatch;
    DrawRectangleBatch rectangleBatch;
    DrawTextureBatch spriteBatch;

    Dimension2D windowSize { 800.f, 600.f };
    static constexpr Dimension2D VIRTUAL_SIZE { 800.f, 600.f };
    
    Viewport worldViewport;

    void renderWorld(RenderContext& ctx);
    void renderShapes(RenderContext& ctx);

    DrawCircleCommand buildCircleCommand(Entity& entity, World& world, size_t order) const;
    DrawRectangleCommand buildRectangleCommand(Entity& entity, World& world, size_t order) const;
    DrawTextureCommand buildTextureCommand(Entity& entity, World& world, size_t order) const;

    Position worldToScreen(Position worldPos, const Viewport& viewport, Position parallax = {1.f, 1.f}) const;
    Position resolveParallax(World& world, Entity entity) const;
    void computeSpriteTransform(const Rectangle& spriteConfig, DrawTextureCommand& cmd) const;
    void updateViewports();
};

#endif // world_drawer_h
