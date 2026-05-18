#ifndef world_drawer_h
#define world_drawer_h

#include "../Camera2D/Camera2D.h"

#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/Drawer/Drawer.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Texture/Texture.h"
#include "../../engine/include/Viewport/Viewport.h"

#include <cstddef>
#include <vector>

struct Entity;

class WorldDrawer : public Drawer
{
public:
    WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera);

    void draw(RenderContext& ctx);

private:
    Renderer& renderer;
    Camera2D& camera;

    int windowWidth = 800;
    int windowHeight = 600;
    static constexpr int VIRTUAL_WIDTH = 800;
    static constexpr int VIRTUAL_HEIGHT = 600;

    Viewport worldViewport;

    void renderWorld(RenderContext& ctx);
    void renderShapes(RenderContext& ctx);

    SpriteCommand buildSpriteCommand(Entity& entity, World& world, size_t order) const;
    Position worldToScreen(Position worldPos, const Viewport& viewport, Position parallax = {1.0f, 1.0f}) const;
    Position resolveParallax(World& world, Entity entity) const;

    void computeSpriteTransform(const Rectangle& spriteConfig, SpriteCommand& cmd) const;
    void sortCommands(std::vector<SpriteCommand>& cmds) const;
    void submitCommands(const std::vector<SpriteCommand>& cmds) const;
    
    void updateViewports();
};

#endif // world_drawer_h
