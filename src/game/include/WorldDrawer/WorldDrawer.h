#ifndef world_drawer_h
#define world_drawer_h

#include "../Camera2D/Camera2D.h"
#include "../IRenderFormat/IRenderFormat.h"

#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/Drawer/Drawer.h"
#include "../../engine/include/EventBus/EventBus.h"
#include "../../engine/include/GameConstants/GameConstants.h"
#include "../../engine/include/GameSettings/GameSettings.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Viewport/Viewport.h"

#include <memory>
#include <vector>

class WorldDrawer : public Drawer
{
public:
    WorldDrawer(EventBus& bus, Renderer& renderer, Camera2D& camera, GameSettings& settings);
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Camera2D& camera;
    GameSettings& settings;

    std::vector<std::unique_ptr<IRenderFormat>> formats;
    Viewport worldViewport {0, 0,
        static_cast<int>(GameConstants::VIRTUAL_SIZE.width),
        static_cast<int>(GameConstants::VIRTUAL_SIZE.height)};

    void addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }

    void recalculateViewport();
    void propagateViewport();
};

#endif // world_drawer_h
