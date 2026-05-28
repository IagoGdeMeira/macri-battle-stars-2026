#ifndef world_texture_render_format_h
#define world_texture_render_format_h

#include "../include/Camera2D/Camera2D.h"
#include "../include/IRenderFormat/IRenderFormat.h"

#include "../../domain/include/Entity/Entity.h"
#include "../../domain/include/World/World.h"

#include "../../engine/include/DrawBatch/DrawTextureBatch.h"
#include "../../engine/include/DrawCommands/DrawCommands.h"
#include "../../engine/include/Renderer/Renderer.h"
#include "../../engine/include/Viewport/Viewport.h"

class WorldTextureRenderFormat : public IRenderFormat
{
public:
    WorldTextureRenderFormat(Renderer& renderer, Camera2D& camera) : renderer(renderer), camera(camera) {}

    void render(RenderContext& ctx) override;
    void setViewport(const Viewport& vp) override { this->viewport = vp; }

private:
    Renderer& renderer;
    Camera2D& camera;
    Viewport viewport;
    DrawTextureBatch batch;

    DrawTextureCommand buildTextureCommand(Entity& entity, World& world, size_t order) const;
};

#endif // world_texture_render_format_h
