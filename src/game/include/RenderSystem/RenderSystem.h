#ifndef render_system_h
#define render_system_h

#include "../Camera2D/Camera2D.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/RenderContext/RenderContext.h"
#include "../../engine/include/Renderer/Renderer.h"
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
        Texture* texture;
        int x, y, width, height;
        float rotation;
        bool flipX, flipY;
        int layer, zIndex;
        size_t order;
        int srcX, srcY, srcWidth, srcHeight;
        bool useSourceRect;
    };

    Renderer& renderer;
    Camera2D& camera;

    int windowWidth = 800;
    int windowHeight = 600;

    const int VIRTUAL_WIDTH = 800;
    const int VIRTUAL_HEIGHT = 600;

    Viewport worldViewport;
    Viewport uiViewport;

    void renderWorld(RenderContext& ctx);
    void renderUI(RenderContext& ctx);

    void worldToScreen(
        float worldX, float worldY,
        float& screenX, float& screenY,
        const Viewport& viewport,
        float parallaxX = 1.0f, float parallaxY = 1.0f);

    void resolveParallaxFactors(
        World& world, Entity entity,
        float& parallaxX, float& parallaxY) const;

    void resolveScaleAndFlip(
        int spriteWidth, int spriteHeight,
        float scaleX, float scaleY,
        int& finalWidth, int& finalHeight,
        bool& flipX, bool& flipY) const;

    void sortDrawCommands(std::vector<DrawCommand>& commands) const;
    void submitDrawCommands(const std::vector<DrawCommand>& commands) const;

    void updateViewports();
};

#endif // render_system_h
