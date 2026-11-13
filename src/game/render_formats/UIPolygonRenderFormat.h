#ifndef ui_polygon_render_format_h
#define ui_polygon_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawPolygonCommand.h"
#include "engine/include/Renderer/Renderer.h"

class UIPolygonRenderFormat : public IRenderFormat
{
public:
    explicit UIPolygonRenderFormat(Renderer& renderer);
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    DrawPolygonCommand buildPolygonCommand(Entity entity, World& world, size_t order) const;
};

#endif // ui_polygon_render_format_h
