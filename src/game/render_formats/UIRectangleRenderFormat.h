#ifndef ui_rectangle_render_format_h
#define ui_rectangle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/Renderer/Renderer.h"

class UIRectangleRenderFormat : public IRenderFormat
{
public:
    explicit UIRectangleRenderFormat(Renderer& renderer) : renderer(renderer) {}
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    DrawRectangleCommand buildRectangleCommand(Entity entity, World& world, size_t order) const;
};

#endif // ui_rectangle_render_format_h
