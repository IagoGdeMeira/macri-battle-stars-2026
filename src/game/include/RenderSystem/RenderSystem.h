#ifndef render_system_h
#define render_system_h

#include "../include/Camera2D/Camera2D.h"

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/System/System.h"

class RenderSystem : public System
{
public:
    RenderSystem(EventBus& bus, Renderer& renderer, Camera2D& camera);

    void update(UpdateContext& ctx) override;

private:
    struct DrawCommand
    {
        Texture* texture;
        int x, y, width, height;
        int layer;
        float sortY;
    };

    Renderer& renderer;
    Camera2D& camera;

    int windowWidth = 800;
    int windowHeight = 600;

    const int VIRTUAL_WIDTH = 800;
    const int VIRTUAL_HEIGHT = 600;

    void worldToScreen(float worldX, float worldY, float &screenX, float &screenY);

    void updateViewport();
};

#endif // render_system_h
