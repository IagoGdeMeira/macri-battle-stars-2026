#ifndef render_system_h
#define render_system_h

#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/System/System.h"

class RenderSystem : public System
{
public:
    RenderSystem(EventBus& bus, Renderer& renderer);

    void update(UpdateContext& ctx) override;

private:
    Renderer& renderer;

    int windowWidth = 800;
    int windowHeight = 600;

    const int VIRTUAL_WIDTH = 800;
    const int VIRTUAL_HEIGHT = 600;

    void updateViewport();
};

#endif // render_system_h
