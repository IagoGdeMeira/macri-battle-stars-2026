#ifndef render_system_h
#define render_system_h

#include "../Camera2D/Camera2D.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/RenderContext/RenderContext.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/GeometryUtils/GeometryUtils.h"
#include "../../engine/include/System/System.h"
#include "../../engine/include/Viewport/Viewport.h"

#include <cstddef>
#include <vector>

struct Entity;

class RenderSystem : public System
{
public:
    RenderSystem(EventBus& bus, Renderer& renderer, Camera2D& camera);

    void update(UpdateContext&) override {}
    void draw(RenderContext& ctx);

private:
    struct DrawCommand
    {
        Texture* texture = nullptr;
        Rectangle dest;
        float rotation = 0.0f;
        bool flipX = false, flipY = false;
        int layer = 0, zIndex = 0;
        size_t order = 0;
        Rectangle source;
        bool useSourceRect = false;
    };

    struct SpriteTransform { int width = 0, height = 0; bool flipX = false, flipY = false; };

    Renderer& renderer;
    Camera2D& camera;

    int windowWidth = 800;
    int windowHeight = 600;
    static constexpr int VIRTUAL_WIDTH = 800;
    static constexpr int VIRTUAL_HEIGHT = 600;

    Viewport worldViewport;
    Viewport uiViewport;

    void renderWorld(RenderContext& ctx);
    void renderShapes(RenderContext& ctx);
    void renderUI(RenderContext& ctx);

    Position worldToScreen(Position worldPos, const Viewport& viewport, Position parallax = {1.0f, 1.0f}) const;
    Position resolveParallax(World& world, Entity entity) const;
    SpriteTransform computeSpriteTransform(int baseWidth, int baseHeight, float scaleX, float scaleY) const;
    DrawCommand buildDrawCommand(Entity entity, World &world, size_t order) const;

    void sortCommands(std::vector<DrawCommand> &commands) const;
    void submitCommands(const std::vector<DrawCommand> &commands) const;
    void updateViewports();
};

#endif // render_system_h
