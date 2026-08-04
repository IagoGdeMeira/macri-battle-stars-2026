#ifndef world_rectangle_render_format_h
#define world_rectangle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_commands/DrawRectangleCommand.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/Camera2D/Camera2D.h"

class WorldRectangleRenderFormat : public IRenderFormat
{
public:
    WorldRectangleRenderFormat(Renderer& renderer, Camera2D& camera) : renderer(renderer), camera(camera) {}
    void render(RenderContext& ctx, RenderQueue& queue) override;

private:
    Renderer& renderer;
    Camera2D& camera;

    DrawRectangleCommand buildRectangleCommand(Entity& entity, World& world, size_t order) const;
};

#endif // world_rectangle_render_format_h
