#ifndef ui_drawer_h
#define ui_drawer_h

#include "../include/IRenderFormat/IRenderFormat.h"

#include "../../../domain/components/OutlineComponent.h"
#include "../../../domain/components/ShadowComponent.h"
#include "../../../domain/components/UITransform.h"
#include "../../../domain/include/Entity/Entity.h"
#include "../../../domain/include/Geometry/Geometry.h"

#include "../../../engine/include/Drawer/Drawer.h"
#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/Viewport/Viewport.h"

#include <cstddef>
#include <memory>
#include <vector>

class UIDrawer : public Drawer
{
public:
    UIDrawer(Renderer& renderer);
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Viewport uiViewport { 0, 0, 800, 600 };

    std::vector<std::unique_ptr<IRenderFormat>> formats;

    void addFormat(std::unique_ptr<IRenderFormat> format)
    { this->formats.push_back(std::move(format)); }
};

#endif // ui_drawer_h
