#ifndef ui_drawer_h
#define ui_drawer_h

#include "../../../domain/components/UITransform.h"
#include "../../../domain/include/Entity/Entity.h"
#include "../../../domain/include/Geometry/Geometry.h"

#include "../../../engine/include/Drawer/Drawer.h"
#include "../../../engine/include/EventBus/EventBus.h"
#include "../../../engine/include/Renderer/Renderer.h"
#include "../../../engine/include/Viewport/Viewport.h"
#include "../../../engine/include/DrawBatch/DrawCircleBatch.h"
#include "../../../engine/include/DrawBatch/DrawFontBatch.h"
#include "../../../engine/include/DrawBatch/DrawRectangleBatch.h"
#include "../../../engine/include/DrawBatch/DrawTextureBatch.h"

#include <cstddef>
#include <vector>

class UIDrawer : public Drawer
{
public:
    UIDrawer(Renderer& renderer) : renderer(renderer) {}
    void draw(RenderContext& ctx) override;

private:
    Renderer& renderer;
    Viewport uiViewport { 0, 0, 800, 600 };

    DrawCircleBatch circleBatch;
    DrawFontBatch fontBatch;
    DrawRectangleBatch rectangleBatch;
    DrawTextureBatch textureBatch;

    void renderSprites(RenderContext& ctx);
    void renderTexts(RenderContext& ctx);
    void renderShapes(RenderContext& ctx);

    DrawTextureCommand buildTextureCommand(Entity& entity, World& world, size_t order) const;
    DrawFontCommand buildFontCommand(Entity& entity, World& world, size_t order) const;
    DrawRectangleCommand buildRectangleCommand(Entity& entity, World& world, size_t order) const;
    DrawCircleCommand buildCircleCommand(Entity& entity, World& world, size_t order) const;

    Position applyAnchor(const UITransform& transform) const;
};

#endif // ui_drawer_h
