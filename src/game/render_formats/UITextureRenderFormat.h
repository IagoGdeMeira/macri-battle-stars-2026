#ifndef ui_texture_render_format_h
#define ui_texture_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawTextureCommand.h"
#include "engine/include/Renderer/Renderer.h"

class UITextureRenderFormat : public IRenderFormat
{
public:
    explicit UITextureRenderFormat(Renderer& renderer) : renderer(renderer) {}
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    DrawTextureCommand buildTextureCommand(Entity entity, World& world, size_t order) const;
};

#endif // ui_texture_render_format_h
