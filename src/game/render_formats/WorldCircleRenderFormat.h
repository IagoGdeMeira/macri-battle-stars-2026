#ifndef world_circle_render_format_h
#define world_circle_render_format_h

#include "IRenderFormat/IRenderFormat.h"

#include "domain/include/Entity/Entity.h"
#include "domain/include/World/World.h"

#include "engine/draw_batches/DrawCircleBatch.h"
#include "engine/draw_commands/DrawCircleCommand.h"
#include "engine/include/Renderer/Renderer.h"
#include "engine/value_objects/Camera2D/Camera2D.h"

class WorldCircleRenderFormat : public IRenderFormat
{
public:
    WorldCircleRenderFormat(Renderer& renderer, Camera2D& camera) : renderer(renderer), camera(camera) {}
    void render(RenderContext& ctx) override;
    
    std::vector<const DrawCommand*> collectCommands() const override;

private:
    Renderer& renderer;
    Camera2D& camera;
    DrawCircleBatch batch;

    DrawCircleCommand buildCircleCommand(Entity& entity, World& world, size_t order) const;
};

#endif // world_circle_render_format_h
