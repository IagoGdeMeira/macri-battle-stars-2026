#ifndef ui_font_render_format_h
#define ui_font_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawFontCommand.h"
#include "engine/include/Renderer/Renderer.h"

class UIFontRenderFormat : public IRenderFormat
{
public:
    explicit UIFontRenderFormat(Renderer& renderer) : renderer(renderer) {}
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    DrawFontCommand buildFontCommand(Entity entity, World& world, size_t order) const;
};

#endif // ui_font_render_format_h
