#ifndef ui_drawer_h
#define ui_drawer_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/components/OutlineComponent.h"
#include "domain/components/ShadowComponent.h"
#include "domain/components/UITransform.h"
#include "domain/include/Entity/Entity.h"
#include "domain/value_objects/Geometry/Geometry.h"

#include "engine/include/Drawer/Drawer.h"
#include "engine/include/EventBus/EventBus.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/GameConstants/GameConstants.h"
#include "engine/value_objects/GameSettings/GameSettings.h"
#include "engine/value_objects/Viewport/Viewport.h"

#include <cstddef>
#include <memory>
#include <vector>

class UIDrawer : public Drawer
{
public:
    UIDrawer(EventBus& bus, Renderer& renderer, GameSettings& settings);
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    GameSettings& settings;

    const Dimension2D vSize = GameConstants::VIRTUAL_SIZE;
    Viewport uiViewport{0, 0, static_cast<int>(this->vSize.width), static_cast<int>(this->vSize.height)};

    std::vector<std::unique_ptr<IRenderFormat>> formats;

    void addFormat(std::unique_ptr<IRenderFormat> format) { this->formats.push_back(std::move(format)); }
    void recalculateViewport();
};

#endif // ui_drawer_h
