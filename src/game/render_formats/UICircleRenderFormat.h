#ifndef ui_circle_render_format_h
#define ui_circle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawCircleCommand.h"
#include "engine/include/Renderer/Renderer.h"

class UICircleRenderFormat : public IRenderFormat
{
public:
    explicit UICircleRenderFormat(Renderer& renderer) : renderer(renderer) {}
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    DrawCircleCommand buildCircleCommand(Entity entity, World& world, size_t order) const;
};

#endif // ui_circle_render_format_h
