#ifndef world_drawer_h
#define world_drawer_h

#include "../Camera2D/Camera2D.h"
#include "../GameSettings/GameSettings.h"
#include "../IRenderFormat/IRenderFormat.h"

#include "../../domain/include/Geometry/Geometry.h"

#include "../../engine/include/Drawer/Drawer.h"
#include "../../engine/include/EventBus/EventBus.h"
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
    GameSettings& settings;
    std::vector<std::unique_ptr<IRenderFormat>> formats;

    Dimension2D windowSize { 800.f, 600.f };
    static constexpr Dimension2D VIRTUAL_SIZE { 800.f, 600.f };
    Viewport worldViewport;

    void addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }
    void updateViewports();
};

#endif // world_drawer_h
